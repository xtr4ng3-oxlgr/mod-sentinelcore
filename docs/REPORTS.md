# Reports

The C++ module writes data to the database and optional local logs.

The Python report builder can create:

- HTML
- JSON

Example:

```bash
python tools/report_builder.py --events sentinelcore_events.csv --metrics sentinelcore_metrics.csv --out reports
```

The dashboard can load a generated report JSON.
