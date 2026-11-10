# DayZ Temperature Zones

Draw temperature zones on a terrain map of DayZ's Nasdara or Chernarus, then export them as the `config.json` for the ZERO_TemperatureZones server mod.

**Open the map: https://zones.bgom.dev/** (Chernarus: https://zones.bgom.dev/?map=chernarusplus)

## Using it

- **World:** pick Nasdara or Chernarus at the left of the toolbar. Each world keeps its own zones and Log setting, and the page opens on the world you used last.
- **Draw:** pick Rectangle or Circle above the map and drag on it. Corners snap to 50 m by default; change that in the toolbar.
- **Edit:** click a zone to drag or resize it, or change its name, shape, mode, temperature, priority, exact edges and height band in the side panel.
- **Check:** the bar under the map shows the coordinates, ground height and ground type wherever you point, and which zone applies there. Where zones overlap, the zone with the highest priority wins; on a tie, the zone whose centre is nearest, as in the mod. Higher priority zones are drawn on top.
- **Export:** the config.json tab shows the finished file, with a Copy button. Save it as UTF-8 without a BOM in the server's profile folder, under `ZERO_TemperatureZones/config.json`, then restart the server.
- **Import:** paste an existing config.json into the box on the same tab to load its zones into the world you're on. This replaces that world's zones.

Zones are saved in your browser only. Nothing is uploaded, so copy the config.json to keep your zones or move them to another device.

The temperature hints use each world's mission: Hardcore Desert's `cfggameplay.json` for Nasdara, and vanilla Chernarus's month temperatures for Chernarus.

## Files

- `index.html`: the page. There is no build step.
- `terrain-nasdara.js`, `terrain-chernarusplus.js`: heights, ground types, buildings and place names, sampled from the game every 25 m. Each world's file is only downloaded when that world is opened.
- `sampler/`: how the terrain files are made.

## Adding a world

1. On a local DayZ server, copy the world's mission folder (for example `dayzOffline.chernarusplus`) to a new folder and replace its `init.c` with `sampler/init.c`.
2. Point a server config's `template` at that folder and start the server. The sampler writes `ztz_terrain.txt` to the profile folder (about a minute on Chernarus), then shuts the server down.
3. Run `node sampler/convert.js <profile>/ztz_terrain.txt terrain-<id>.js <id> <Label>`, where `<id>` is the world name the sampler printed on its first line.
4. Add the world to `MAPS` in `index.html`: its label, mission name, month temperatures, legend, and a `classify` that sorts its surface names (listed in the `SURF` lines) into the ground types.

DayZ, Nasdara and Chernarus belong to Bohemia Interactive. This is an unofficial fan tool.
