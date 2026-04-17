from pathlib import Path
import os
import subprocess
import struct
import sys

import pytest

from Guard_ring_aware_HB_Tree import visualize_placement
from Guard_ring_aware_HB_Tree.visualize_placement import (
    PlacementRecord,
    _canvas_size_for_layout,
    nm_to_um,
    parse_placement_file,
    render_png,
    select_largest_rectangles,
    _style_for_group,
)


def _png_size(path: Path) -> tuple[int, int]:
    data = path.read_bytes()
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        raise AssertionError("not a PNG file")
    if data[12:16] != b"IHDR":
        raise AssertionError("missing PNG IHDR chunk")
    return struct.unpack(">II", data[16:24])


def test_parse_placement_file_reads_header_and_records(tmp_path: Path) -> None:
    placement = tmp_path / "placement.txt"
    placement.write_text(
        "\n".join(
            [
                "3",
                "120 120",
                "0 10 20 30 40 M1",
                "1 15 25 35 45 M2",
                "0 11 21 31 41 M1",
            ]
        )
        + "\n"
    )

    count, width, height, records = parse_placement_file(placement)

    assert count == 3
    assert width == 120
    assert height == 120
    assert records == [
        PlacementRecord(0, 10, 20, 30, 40, "M1"),
        PlacementRecord(1, 15, 25, 35, 45, "M2"),
        PlacementRecord(0, 11, 21, 31, 41, "M1"),
    ]


def test_select_largest_rectangles_keeps_largest_area_per_name() -> None:
    records = [
        PlacementRecord(0, 0, 0, 10, 10, "M1"),
        PlacementRecord(1, 1, 1, 12, 9, "M1"),
        PlacementRecord(0, 2, 2, 8, 8, "M2"),
        PlacementRecord(1, 3, 3, 7, 10, "M2"),
    ]

    selected = select_largest_rectangles(records)

    assert selected == {
        "M1": PlacementRecord(1, 1, 1, 12, 9, "M1"),
        "M2": PlacementRecord(1, 3, 3, 7, 10, "M2"),
    }


def test_parse_placement_file_rejects_malformed_header(tmp_path: Path) -> None:
    placement = tmp_path / "placement_bad.txt"
    placement.write_text(
        "\n".join(
            [
                "not-an-int",
                "120 120",
                "0 10 20 30 40 M1",
            ]
        )
        + "\n"
    )

    with pytest.raises(ValueError):
        parse_placement_file(placement)


def test_render_png_writes_png_file(tmp_path: Path) -> None:
    output_path = tmp_path / "placement.png"
    records = [
        PlacementRecord(0, 0, 0, 20, 10, "M1"),
        PlacementRecord(1, 25, 5, 15, 20, "M2"),
    ]

    render_png(50, 40, records, output_path)

    assert output_path.exists()
    assert output_path.read_bytes().startswith(b"\x89PNG\r\n\x1a\n")


def test_style_for_group_distinguishes_guard_ring_and_device() -> None:
    guard_ring_style = _style_for_group(0, 0)
    device_style = _style_for_group(1, 0)

    assert guard_ring_style["facecolor"] != device_style["facecolor"]
    assert guard_ring_style["alpha"] < device_style["alpha"]
    assert guard_ring_style["label_fontsize"] < device_style["label_fontsize"]


def test_style_for_group_rejects_unsupported_group() -> None:
    with pytest.raises(ValueError):
        _style_for_group(2, 0)


def test_canvas_size_for_layout_caps_pixels_after_scaling() -> None:
    assert _canvas_size_for_layout(100.0, 100.0) == (2048, 2048)


def test_render_png_draws_guard_rings_before_devices_and_labels_last(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    import matplotlib.axes

    output_path = tmp_path / "placement.png"
    records = [
        PlacementRecord(1, 10, 10, 20, 20, "device"),
        PlacementRecord(0, 0, 0, 40, 40, "guard"),
    ]

    call_order: list[tuple[str, str]] = []
    original_add_patch = matplotlib.axes.Axes.add_patch
    original_text = matplotlib.axes.Axes.text

    def spy_add_patch(self, patch, *args, **kwargs):  # type: ignore[no-untyped-def]
        if patch.get_fill():
            call_order.append(
                (
                    "patch",
                    f"{patch.get_alpha()}:{patch.get_linestyle()}:{patch.get_facecolor()}",
                )
            )
        return original_add_patch(self, patch, *args, **kwargs)

    def spy_text(self, *args, **kwargs):  # type: ignore[no-untyped-def]
        call_order.append(("text", str(args[2])))
        return original_text(self, *args, **kwargs)

    monkeypatch.setattr(matplotlib.axes.Axes, "add_patch", spy_add_patch)
    monkeypatch.setattr(matplotlib.axes.Axes, "text", spy_text)

    render_png(50, 40, records, output_path)

    patch_calls = [entry for entry in call_order if entry[0] == "patch"]
    text_calls = [entry for entry in call_order if entry[0] == "text"]

    assert patch_calls[0][1].startswith("0.2:--")
    assert patch_calls[1][1].startswith("0.55:-")
    assert len(text_calls) == 2
    assert call_order.index(text_calls[0]) > call_order.index(patch_calls[1])
    assert call_order.index(text_calls[1]) > call_order.index(patch_calls[1])


def test_render_png_rejects_invalid_layout_or_empty_records(tmp_path: Path) -> None:
    output_path = tmp_path / "placement.png"

    with pytest.raises(ValueError):
        render_png(0, 40, [PlacementRecord(0, 0, 0, 10, 10, "M1")], output_path)

    with pytest.raises(ValueError):
        render_png(50, 40, [], output_path)


def test_render_png_rejects_out_of_bounds_or_negative_geometry(tmp_path: Path) -> None:
    output_path = tmp_path / "placement.png"

    with pytest.raises(ValueError):
        render_png(50, 40, [PlacementRecord(0, -1, 0, 10, 10, "M1")], output_path)

    with pytest.raises(ValueError):
        render_png(50, 40, [PlacementRecord(0, 0, 0, 51, 10, "M1")], output_path)

    with pytest.raises(ValueError):
        render_png(50, 40, [PlacementRecord(0, 0, 35, 10, 6, "M1")], output_path)


def test_nm_to_um_converts_nanometers_to_micrometers() -> None:
    assert nm_to_um(0) == 0.0
    assert nm_to_um(1000) == 1.0
    assert nm_to_um(2500) == 2.5


def test_render_png_uses_micrometer_axes_and_five_um_ticks(tmp_path: Path) -> None:
    import matplotlib.backends.backend_agg as backend_agg

    output_path = tmp_path / "placement.png"
    records = [PlacementRecord(0, 0, 0, 5000, 10000, "M1")]

    captured: dict[str, object] = {}
    original_print_png = backend_agg.FigureCanvasAgg.print_png

    def spy_print_png(self, filename, *args, **kwargs):  # type: ignore[no-untyped-def]
        captured["axes"] = self.figure.axes[0]
        return original_print_png(self, filename, *args, **kwargs)

    backend_agg.FigureCanvasAgg.print_png = spy_print_png  # type: ignore[assignment]
    try:
        render_png(20000, 15000, records, output_path)
    finally:
        backend_agg.FigureCanvasAgg.print_png = original_print_png  # type: ignore[assignment]

    assert output_path.exists()
    assert output_path.read_bytes().startswith(b"\x89PNG\r\n\x1a\n")
    axes = captured["axes"]
    assert axes.get_xlim() == (0.0, 20.0)
    assert axes.get_ylim() == (0.0, 15.0)
    assert axes.get_xticks().tolist() == [0.0, 5.0, 10.0, 15.0, 20.0]
    assert axes.get_yticks().tolist() == [0.0, 5.0, 10.0, 15.0]
    assert axes.get_xlabel() == "um"
    assert axes.get_ylabel() == "um"


def test_select_largest_rectangles_rejects_invalid_geometry() -> None:
    records = [
        PlacementRecord(0, 0, 0, 10, 10, "M1"),
        PlacementRecord(1, -1, 0, 10, 10, "M2"),
    ]

    with pytest.raises(ValueError):
        select_largest_rectangles(records)


def test_importing_module_does_not_touch_mplconfigdir_or_create_it(tmp_path: Path) -> None:
    mplconfigdir = tmp_path / "mplconfig"
    script = """
import os
from pathlib import Path

mplconfigdir = Path(r"{mplconfigdir}")
os.environ["MPLCONFIGDIR"] = str(mplconfigdir)
from Guard_ring_aware_HB_Tree import visualize_placement

print(os.environ["MPLCONFIGDIR"])
print(mplconfigdir.exists())
""".format(mplconfigdir=mplconfigdir)
    env = os.environ.copy()

    completed = subprocess.run(
        [sys.executable, "-c", script],
        cwd=Path(__file__).resolve().parents[2],
        capture_output=True,
        text=True,
        env=env,
        check=True,
    )

    lines = completed.stdout.strip().splitlines()
    assert lines == [str(mplconfigdir), "False"]


def test_cli_writes_png_next_to_input_file(tmp_path: Path) -> None:
    source_placement = Path(__file__).resolve().parents[1] / "opamp_right_out.txt"
    placement = tmp_path / "opamp_right_out.txt"
    placement.write_text(source_placement.read_text())
    output_path = tmp_path / "picture" / "opamp_right_out.png"
    if output_path.exists():
        output_path.unlink()

    completed = subprocess.run(
        [sys.executable, str(Path(__file__).resolve().parents[1] / "visualize_placement.py"), str(placement)],
        cwd=Path(__file__).resolve().parents[2],
        capture_output=True,
        text=True,
    )

    assert completed.returncode == 0, completed.stderr
    assert completed.stderr == ""
    assert output_path.exists()
    assert output_path.read_bytes().startswith(b"\x89PNG\r\n\x1a\n")
    width, height = _png_size(output_path)
    assert max(width, height) <= 4096


def test_cli_renders_all_records_in_input_order(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    placement = tmp_path / "placement.txt"
    placement.write_text(
        "\n".join(
            [
                "3",
                "120 120",
                "0 10 20 30 40 M1",
                "1 15 25 35 45 M1",
                "0 11 21 31 41 M2",
            ]
        )
        + "\n"
    )

    rendered: list[PlacementRecord] = []

    def fake_render_png(width: int, height: int, records: list[PlacementRecord], output_path: Path) -> None:
        rendered.extend(records)

    monkeypatch.setattr(visualize_placement, "render_png", fake_render_png)

    assert visualize_placement.main([str(placement)]) == 0
    assert rendered == [
        PlacementRecord(0, 10, 20, 30, 40, "M1"),
        PlacementRecord(1, 15, 25, 35, 45, "M1"),
        PlacementRecord(0, 11, 21, 31, 41, "M2"),
    ]


def test_cli_reports_missing_input_file() -> None:
    missing = Path(__file__).resolve().parents[1] / "does-not-exist.txt"

    completed = subprocess.run(
        [sys.executable, str(Path(__file__).resolve().parents[1] / "visualize_placement.py"), str(missing)],
        cwd=Path(__file__).resolve().parents[2],
        capture_output=True,
        text=True,
    )

    assert completed.returncode != 0
    assert completed.stderr.strip() == f"error: input file not found: {missing}"
