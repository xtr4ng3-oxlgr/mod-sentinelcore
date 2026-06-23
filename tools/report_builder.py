#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SentinelCore report builder.

Reads exported CSV files from the character database and generates
HTML/JSON reports.

Created by xtr4ng3.
"""

from __future__ import annotations

import argparse
import csv
import datetime as dt
import html
import json
from collections import Counter, defaultdict
from pathlib import Path


def read_csv(path: Path) -> list[dict]:
    if not path.exists():
        return []
    with path.open("r", encoding="utf-8", errors="ignore", newline="") as f:
        return list(csv.DictReader(f))


def severity_score(sev: str) -> int:
    sev = (sev or "").lower()
    return {
        "info": 0,
        "low": 10,
        "medium": 35,
        "high": 70,
        "critical": 100,
    }.get(sev, 0)


def build_report(events: list[dict], metrics: list[dict]) -> dict:
    by_type = Counter(e.get("event_type", "unknown") for e in events)
    by_sev = Counter(e.get("severity", "unknown") for e in events)
    by_player = Counter(e.get("player_name", "") for e in events if e.get("player_name"))
    by_action = Counter(e.get("action", "") for e in events if e.get("action"))

    score = 0
    for e in events:
        score = max(score, severity_score(e.get("severity", "")))

    if by_sev.get("high", 0) >= 10:
        score = max(score, 75)
    if by_sev.get("critical", 0) > 0:
        score = 100

    verdict = "clean"
    if score >= 90:
        verdict = "critical"
    elif score >= 70:
        verdict = "high"
    elif score >= 40:
        verdict = "medium"
    elif score > 0:
        verdict = "low"

    return {
        "generated_at": dt.datetime.now().isoformat(timespec="seconds"),
        "score": score,
        "verdict": verdict,
        "total_events": len(events),
        "total_metrics": len(metrics),
        "events_by_type": dict(by_type),
        "events_by_severity": dict(by_sev),
        "top_players": by_player.most_common(10),
        "top_actions": by_action.most_common(10),
        "recent_events": events[-50:],
        "metrics": metrics[-100:],
    }


def write_json(report: dict, path: Path) -> None:
    path.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding="utf-8")


def write_html(report: dict, path: Path) -> None:
    def table_counter(title: str, data):
        rows = "".join(f"<tr><td>{html.escape(str(k))}</td><td>{v}</td></tr>" for k, v in data)
        return f"<div class='card'><h2>{html.escape(title)}</h2><table><tr><th>Name</th><th>Count</th></tr>{rows}</table></div>"

    recent_rows = "".join(
        "<tr>"
        f"<td>{html.escape(e.get('created_at',''))}</td>"
        f"<td>{html.escape(e.get('severity',''))}</td>"
        f"<td>{html.escape(e.get('event_type',''))}</td>"
        f"<td>{html.escape(e.get('player_name',''))}</td>"
        f"<td>{html.escape(e.get('category',''))}</td>"
        f"<td>{html.escape(e.get('action',''))}</td>"
        f"<td>{html.escape(e.get('detail',''))}</td>"
        "</tr>"
        for e in report["recent_events"]
    )

    doc = f"""<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<title>SentinelCore Report</title>
<style>
body{{background:#05070b;color:#e8f6ff;font-family:Consolas,Segoe UI,Arial;padding:30px}}
h1,h2{{color:#ff304f}}
.card{{background:#0b1018;border:1px solid #202c3f;border-radius:16px;padding:18px;margin:18px 0}}
table{{width:100%;border-collapse:collapse;margin-top:12px}}
td,th{{border-bottom:1px solid #1a2130;padding:9px;text-align:left;vertical-align:top}}
th{{color:#7ef9ff}}
.score{{font-size:54px;font-weight:900;color:#ff304f}}
.small{{color:#9fb1c7}}
</style>
</head>
<body>
<h1>SentinelCore</h1>
<p class="small">AzerothCore observability report · xtr4ng3 · {html.escape(report["generated_at"])}</p>

<div class="card">
<h2>Verdict</h2>
<div class="score">{report["score"]}/100</div>
<p><b>{html.escape(report["verdict"].upper())}</b></p>
<p>Total events: {report["total_events"]} · Total metrics: {report["total_metrics"]}</p>
</div>

{table_counter("Events by type", report["events_by_type"].items())}
{table_counter("Events by severity", report["events_by_severity"].items())}
{table_counter("Top players", report["top_players"])}
{table_counter("Top actions", report["top_actions"])}

<div class="card">
<h2>Recent events</h2>
<table>
<tr><th>Created</th><th>Severity</th><th>Type</th><th>Player</th><th>Category</th><th>Action</th><th>Detail</th></tr>
{recent_rows}
</table>
</div>

</body>
</html>"""
    path.write_text(doc, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser(description="Build SentinelCore HTML/JSON reports from exported CSV files.")
    parser.add_argument("--events", default="sentinelcore_events.csv")
    parser.add_argument("--metrics", default="sentinelcore_metrics.csv")
    parser.add_argument("--out", default="reports")
    args = parser.parse_args()

    out = Path(args.out)
    out.mkdir(parents=True, exist_ok=True)

    events = read_csv(Path(args.events))
    metrics = read_csv(Path(args.metrics))
    report = build_report(events, metrics)

    stamp = dt.datetime.now().strftime("%Y%m%d_%H%M%S")
    write_json(report, out / f"sentinelcore_{stamp}.json")
    write_html(report, out / f"sentinelcore_{stamp}.html")

    print(f"Report written to {out}")


if __name__ == "__main__":
    main()
