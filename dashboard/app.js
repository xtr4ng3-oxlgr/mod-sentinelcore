const file = document.getElementById("file");

function dump(obj) {
  return JSON.stringify(obj, null, 2);
}

file.addEventListener("change", async () => {
  const f = file.files[0];
  if (!f) return;

  const text = await f.text();
  const report = JSON.parse(text);

  document.getElementById("score").textContent = report.score ?? "--";
  document.getElementById("events").textContent =
    `total_events: ${report.total_events}\nverdict: ${report.verdict}\ngenerated_at: ${report.generated_at}`;

  document.getElementById("severity").textContent = dump(report.events_by_severity || {});
  document.getElementById("players").textContent = dump(report.top_players || []);
  document.getElementById("actions").textContent = dump(report.top_actions || []);
});
