from __future__ import annotations

import os
import tempfile
import sys
from dataclasses import dataclass
from math import ceil
from pathlib import Path


@dataclass(frozen=True)
class PlacementRecord:
    group: int
    x: int
    y: int
    width: int
    height: int
    name: str


def nm_to_um(value: int | float) -> float:
    return value / 1000.0


def parse_placement_file(path: Path) -> tuple[int, int, int, list[PlacementRecord]]:
    lines = [line.strip() for line in path.read_text().splitlines() if line.strip()]
    if len(lines) < 2:
        raise ValueError("placement file header is incomplete")

    try:
        count = int(lines[0])
        width_str, height_str = lines[1].split()
        width = int(width_str)
        height = int(height_str)
    except (ValueError, IndexError) as exc:
        raise ValueError("invalid placement file header") from exc

    records: list[PlacementRecord] = []
    for line in lines[2:]:
        parts = line.split()
        if len(parts) < 6:
            raise ValueError("invalid placement record")
        try:
            group, x, y, rec_width, rec_height = map(int, parts[:5])
        except ValueError as exc:
            raise ValueError("invalid placement record") from exc
        name = " ".join(parts[5:])
        records.append(PlacementRecord(group, x, y, rec_width, rec_height, name))

    if count != len(records):
        raise ValueError("placement record count does not match header")

    return count, width, height, records


def select_largest_rectangles(records: list[PlacementRecord]) -> dict[str, PlacementRecord]:
    selected: dict[str, PlacementRecord] = {}
    best_area: dict[str, int] = {}

    for record in records:
        _validate_record_geometry(record)
        area = record.width * record.height
        if record.name not in selected or area > best_area[record.name]:
            selected[record.name] = record
            best_area[record.name] = area

    return selected


def _validate_record_geometry(
    record: PlacementRecord,
    layout_width: int | None = None,
    layout_height: int | None = None,
) -> None:
    if record.width <= 0 or record.height <= 0:
        raise ValueError("rectangle dimensions must be positive")
    if record.x < 0 or record.y < 0:
        raise ValueError("rectangle coordinates must be non-negative")
    if layout_width is not None and record.x + record.width > layout_width:
        raise ValueError("rectangle exceeds layout width")
    if layout_height is not None and record.y + record.height > layout_height:
        raise ValueError("rectangle exceeds layout height")


def _tick_positions(max_value_um: float, interval_um: int = 5) -> list[float]:
    if max_value_um < 0:
        raise ValueError("axis range must be non-negative")
    end = ceil(max_value_um / interval_um) * interval_um
    return [float(tick) for tick in range(0, int(end) + interval_um, interval_um)]


def _style_for_group(group: int, index: int) -> dict[str, object]:
    if group == 0:
        return {
            "facecolor": "#d1d5db",
            "edgecolor": "#6b7280",
            "alpha": 0.2,
            "linewidth": 0.8,
            "linestyle": "--",
            "label_fontsize": 9,
            "text_color": "#374151",
        }

    if group == 1:
        return {
            "facecolor": f"C{index % 10}",
            "edgecolor": "#111827",
            "alpha": 0.55,
            "linewidth": 1.2,
            "linestyle": "-",
            "label_fontsize": 11,
            "text_color": "black",
        }

    raise ValueError(f"unsupported placement group: {group}")


def render_png(
    layout_width: int,
    layout_height: int,
    records: list[PlacementRecord],
    output_path: Path,
) -> None:
    _configure_matplotlib()

    import matplotlib

    matplotlib.use("Agg", force=True)

    from matplotlib.backends.backend_agg import FigureCanvasAgg
    from matplotlib.figure import Figure
    from matplotlib.patches import Patch, Rectangle
    from matplotlib.ticker import MultipleLocator

    if layout_width <= 0 or layout_height <= 0:
        raise ValueError("layout dimensions must be positive")
    if not records:
        raise ValueError("no records available for rendering")

    layout_width_um = nm_to_um(layout_width)
    layout_height_um = nm_to_um(layout_height)
    canvas_width, canvas_height = _canvas_size_for_layout(layout_width_um, layout_height_um)
    dpi = 100
    figure = Figure(figsize=(canvas_width / dpi, canvas_height / dpi), dpi=dpi)
    canvas = FigureCanvasAgg(figure)
    axes = figure.add_axes([0.08, 0.08, 0.88, 0.88])
    axes.set_xlim(0, layout_width_um)
    axes.set_ylim(0, layout_height_um)
    axes.set_aspect("equal", adjustable="box")
    axes.set_axis_on()
    axes.xaxis.set_major_locator(MultipleLocator(5))
    axes.yaxis.set_major_locator(MultipleLocator(5))
    axes.set_xticks(_tick_positions(layout_width_um))
    axes.set_yticks(_tick_positions(layout_height_um))
    axes.set_xlabel("um")
    axes.set_ylabel("um")

    figure.patch.set_facecolor("white")
    for record in records:
        _validate_record_geometry(record, layout_width, layout_height)
    axes.add_patch(
        Rectangle(
            (0, 0),
            layout_width_um,
            layout_height_um,
            fill=False,
            edgecolor="#444444",
            linewidth=1.5,
        )
    )

    ordered_records = sorted(
        enumerate(records),
        key=lambda item: (0 if item[1].group == 0 else 1, item[0]),
    )

    for index, record in ordered_records:
        style = _style_for_group(record.group, index)
        axes.add_patch(
            Rectangle(
                (nm_to_um(record.x), nm_to_um(record.y)),
                nm_to_um(record.width),
                nm_to_um(record.height),
                facecolor=style["facecolor"],
                edgecolor=style["edgecolor"],
                linewidth=style["linewidth"],
                alpha=style["alpha"],
                linestyle=style["linestyle"],
            )
        )

    device_label_names = {record.name for record in records if record.group == 1}

    for index, record in ordered_records:
        if record.group == 0 and record.name in device_label_names:
            continue
        style = _style_for_group(record.group, index)
        axes.text(
            nm_to_um(record.x + record.width / 2),
            nm_to_um(record.y + record.height / 2),
            record.name,
            ha="center",
            va="center",
            fontsize=style["label_fontsize"],
            color=style["text_color"],
        )

    _draw_legend(axes, records, Patch)

    canvas.print_png(str(output_path))


def _draw_legend(axes, records: list[PlacementRecord], Patch) -> None:
    handles = []
    if any(r.group == 0 for r in records):
        s = _style_for_group(0, 0)
        handles.append(Patch(facecolor=s["facecolor"], edgecolor=s["edgecolor"],
                             linestyle=s["linestyle"], linewidth=s["linewidth"],
                             alpha=s["alpha"], label="Guard ring"))
    if any(r.group == 1 for r in records):
        s = _style_for_group(1, 0)
        handles.append(Patch(facecolor="#9ca3af", edgecolor=s["edgecolor"],
                             linestyle=s["linestyle"], linewidth=s["linewidth"],
                             alpha=s["alpha"], label="Device"))
    if handles:
        axes.legend(handles=handles, loc="upper right", framealpha=0.9, fontsize=10)


def _output_path_for(input_path: Path) -> Path:
    return input_path.parent / "picture" / f"{input_path.stem}.png"


def _configure_matplotlib() -> None:
    mplconfigdir = os.environ.get("MPLCONFIGDIR")
    if mplconfigdir:
        Path(mplconfigdir).mkdir(parents=True, exist_ok=True)
        return

    mplconfigdir_path = Path(tempfile.gettempdir()) / "matplotlib"
    mplconfigdir_path.mkdir(parents=True, exist_ok=True)
    os.environ["MPLCONFIGDIR"] = str(mplconfigdir_path)


def _canvas_size_for_layout(
    layout_width: float,
    layout_height: float,
    max_dimension: int = 2048,
    pixels_per_unit: int = 100,
) -> tuple[int, int]:
    canvas_width = max(1, round(layout_width * pixels_per_unit))
    canvas_height = max(1, round(layout_height * pixels_per_unit))
    longest_side = max(canvas_width, canvas_height)
    if longest_side <= max_dimension:
        return canvas_width, canvas_height

    scale = max_dimension / longest_side
    canvas_width = max(1, round(layout_width * pixels_per_unit * scale))
    canvas_height = max(1, round(layout_height * pixels_per_unit * scale))
    return canvas_width, canvas_height


def main(argv: list[str] | None = None) -> int:
    args = sys.argv[1:] if argv is None else argv
    if len(args) != 1:
        print("usage: python visualize_placement.py <placement.txt>", file=sys.stderr)
        return 2

    input_path = Path(args[0])
    output_path = _output_path_for(input_path)

    try:
        if not input_path.exists():
            raise FileNotFoundError(input_path)
        _, width, height, records = parse_placement_file(input_path)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        render_png(width, height, records, output_path)
    except FileNotFoundError:
        print(f"error: input file not found: {input_path}", file=sys.stderr)
        return 1
    except (OSError, ValueError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
