# Nasdara Temperature Zones

Draw temperature zones on a terrain map of DayZ's Nasdara, then export them as the `config.json` for the NasdaraHardcore_TemperatureZones server mod.

**Open the map: https://joearcher.github.io/nasdara-temperature-zones/**

## Using it

- **Draw:** pick Rectangle or Circle above the map and drag on it. Corners snap to 50 m by default; change that in the toolbar.
- **Edit:** click a zone to drag or resize it, or change its name, shape, mode, temperature, exact edges and height band in the side panel.
- **Check:** the bar under the map shows the coordinates, ground height and ground type wherever you point, and which zone applies there. Where zones overlap, the zone whose centre is nearest wins, as in the mod.
- **Export:** the config.json tab shows the finished file, with a Copy button. Save it as UTF-8 without a BOM in the server's profile folder, under `NasdaraHardcore_TemperatureZones/config.json`, then restart the server.
- **Import:** paste an existing config.json into the box on the same tab to load its zones. This replaces the zones on the page.

Zones are saved in your browser only. Nothing is uploaded, so copy the config.json to keep your zones or move them to another device.

## Files

- `index.html`: the page. There is no build step.
- `terrain.js`: heights, ground types, buildings and place names, sampled from the game every 25 m.

DayZ and Nasdara belong to Bohemia Interactive. This is an unofficial fan tool.
