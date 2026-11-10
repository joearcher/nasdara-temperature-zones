// Turns the sampler's ztz_terrain.txt into terrain-<id>.js for the zones tool
// usage: node convert.js <ztz_terrain.txt> <out.js> <id> <label>
const fs = require("fs");
const [inFile, outFile, id, label] = process.argv.slice(2);
const lines = fs.readFileSync(inFile, "utf8").split(/\r?\n/);

let size, step, n;
const hgt = [], srf = [], surfaces = [], bld = [], names = [];
const KEEP = new Set(["Capital", "City", "Village", "Local", "Hill", "Marine", "Ruin", "Camp"]);
// English name from the config class: Settlement_NovayaPetrovka -> Novaya Petrovka
function english(cls, type) {
  let s = cls.replace(/^[A-Za-z]+_/, "").replace(/\d+$/, "").replace(/_/g, " ");
  s = s.replace(/([a-z])([A-Z])/g, "$1 $2");
  if (type === "Camp" && !/camp/i.test(s)) s = "Camp " + s;
  return s.trim();
}

for (const line of lines) {
  if (line.startsWith("META ")) { const p = line.split(" "); size = +p[1]; step = +p[2]; n = +p[3]; }
  else if (line.startsWith("H ")) hgt.push(...line.slice(2).trim().split(" ").map(Number));
  else if (line.startsWith("S ")) srf.push(...line.slice(2).trim().split(" ").map(Number));
  else if (line.startsWith("SURF ")) { const m = line.match(/^SURF (\d+) (.*)$/); surfaces[+m[1]] = m[2]; }
  else if (line.startsWith("B ")) { const p = line.split(" "); bld.push(+p[1], +p[2]); }
  else if (line.startsWith("N ")) {
    const [type, x, z, , cls] = line.slice(2).split("|");
    if (KEEP.has(type)) names.push([type, +x, +z, english(cls, type)]);
  }
}
if (hgt.length !== n * n || srf.length !== n * n) throw new Error(`expected ${n * n} samples, got ${hgt.length} heights and ${srf.length} surfaces`);
if (!lines.includes("DONE")) throw new Error("sampler output is incomplete");

const b64 = (typed) => Buffer.from(typed.buffer).toString("base64");
const data = {
  size, step, n, surfaces,
  hgt: b64(Int16Array.from(hgt)),
  srf: b64(Uint8Array.from(srf)),
  bld: b64(Uint16Array.from(bld)),
  names
};
const out = `// ${label} terrain: heights, ground types, buildings and place names, sampled from the game every ${step} m\n` +
  `(window.ZONE_TERRAIN = window.ZONE_TERRAIN || {}).${id} = ${JSON.stringify(data)};\n`;
fs.writeFileSync(outFile, out);
console.log(`${outFile}: ${n} x ${n}, ${surfaces.length} surfaces, ${bld.length / 2} buildings, ${names.length} names, ${out.length} bytes`);
