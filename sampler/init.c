// Terrain sampler for the temperature zones map tool. Samples heights, ground types and water every STEP metres,
// lists buildings and place names, writes $profile:ztz_terrain.txt, then shuts the server down.

class ZTZ_TerrainSampler
{
	static const int STEP = 25;
	static const int ROWS_PER_TICK = 8;
	static const int TILE = 200;
	static const int TILES_PER_TICK = 6;
	static const string OUT = "$profile:ztz_terrain.txt";

	int m_Size;
	int m_N;
	int m_Row;
	int m_TileX;
	int m_TileZ;
	int m_Buildings;
	FileHandle m_File;
	ref map<string, int> m_Surfaces = new map<string, int>();
	ref array<string> m_SurfaceNames = new array<string>();

	void Start()
	{
		m_Size = GetGame().GetWorld().GetWorldSize();
		m_N = Math.Floor(m_Size / STEP) + 1;
		string world;
		GetGame().GetWorldName(world);
		m_File = OpenFile(OUT, FileMode.WRITE);
		if (!m_File)
		{
			Print("[ZTZ_Sampler] could not open " + OUT);
			return;
		}
		FPrintln(m_File, string.Format("META %1 %2 %3 %4", m_Size, STEP, m_N, world));
		Print(string.Format("[ZTZ_Sampler] start: %1, size %2, %3 x %3 samples", world, m_Size, m_N));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SampleRows, 10, false);
	}

	int SurfaceIndex(string name)
	{
		int idx;
		if (m_Surfaces.Find(name, idx))
			return idx;
		idx = m_SurfaceNames.Count();
		m_SurfaceNames.Insert(name);
		m_Surfaces.Insert(name, idx);
		return idx;
	}

	// Row 0 is the north edge, as the tool expects
	void SampleRows()
	{
		for (int i = 0; i < ROWS_PER_TICK && m_Row < m_N; i++)
		{
			float z = (m_N - 1 - m_Row) * STEP;
			string hLine = "H";
			string sLine = "S";
			for (int c = 0; c < m_N; c++)
			{
				float x = c * STEP;
				int h = Math.Round(GetGame().SurfaceY(x, z) * 10);
				int s;
				if (GetGame().SurfaceIsSea(x, z) || GetGame().SurfaceIsPond(x, z))
				{
					s = 255;
				}
				else
				{
					string type;
					GetGame().SurfaceGetType(x, z, type);
					if (type == "")
						type = "(none)";
					s = SurfaceIndex(type);
				}
				hLine += " " + h;
				sLine += " " + s;
			}
			FPrintln(m_File, hLine);
			FPrintln(m_File, sLine);
			m_Row++;
		}

		if (m_Row % 80 == 0)
			Print(string.Format("[ZTZ_Sampler] rows %1 / %2", m_Row, m_N));

		if (m_Row < m_N)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SampleRows, 1, false);
			return;
		}

		for (int j = 0; j < m_SurfaceNames.Count(); j++)
			FPrintln(m_File, string.Format("SURF %1 %2", j, m_SurfaceNames[j]));

		Print("[ZTZ_Sampler] terrain done, " + m_SurfaceNames.Count() + " surface types; buildings next");
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SampleBuildings, 10, false);
	}

	bool IsBuilding(Object obj)
	{
		if (!obj || !obj.IsInherited(House))
			return false;
		string type = obj.GetType();
		if (type.IndexOf("Land_") != 0)
			return false;
		type.ToLower();
		array<string> skip = {"wall", "fence", "misc", "power", "pole", "rail", "pipe", "tank", "pier", "bridge", "sign", "lamp", "wreck", "rubble", "garbage", "dirtpile", "container", "boat", "ship"};
		foreach (string word : skip)
		{
			if (type.IndexOf(word) != -1)
				return false;
		}
		return true;
	}

	void SampleBuildings()
	{
		int tiles = Math.Ceil(m_Size / TILE);
		for (int i = 0; i < TILES_PER_TICK && m_TileZ < tiles; i++)
		{
			float x0 = m_TileX * TILE;
			float z0 = m_TileZ * TILE;
			vector centre = Vector(x0 + TILE * 0.5, 0, z0 + TILE * 0.5);
			centre[1] = GetGame().SurfaceY(centre[0], centre[2]);
			array<Object> objects = new array<Object>();
			GetGame().GetObjectsAtPosition(centre, TILE * 0.75, objects, null);
			foreach (Object obj : objects)
			{
				if (!IsBuilding(obj))
					continue;
				vector p = obj.GetPosition();
				// Only the tile that owns the position, so no building is listed twice
				if (p[0] < x0 || p[0] >= x0 + TILE || p[2] < z0 || p[2] >= z0 + TILE)
					continue;
				FPrintln(m_File, string.Format("B %1 %2", Math.Round(p[0]), Math.Round(p[2])));
				m_Buildings++;
			}

			m_TileX++;
			if (m_TileX >= tiles)
			{
				m_TileX = 0;
				m_TileZ++;
				if (m_TileZ % 10 == 0)
					Print(string.Format("[ZTZ_Sampler] building rows %1 / %2, %3 buildings", m_TileZ, tiles, m_Buildings));
			}
		}

		if (m_TileZ < tiles)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SampleBuildings, 1, false);
			return;
		}

		SampleNames();
	}

	void SampleNames()
	{
		string world;
		GetGame().GetWorldName(world);
		string root = "CfgWorlds " + world + " Names";
		int count = GetGame().ConfigGetChildrenCount(root);
		for (int i = 0; i < count; i++)
		{
			string child;
			GetGame().ConfigGetChildName(root, i, child);
			string path = root + " " + child;
			string name;
			string type;
			GetGame().ConfigGetText(path + " name", name);
			GetGame().ConfigGetText(path + " type", type);
			TFloatArray pos = new TFloatArray();
			GetGame().ConfigGetFloatArray(path + " position", pos);
			if (pos.Count() < 2)
				continue;
			FPrintln(m_File, string.Format("N %1|%2|%3|%4|%5", type, Math.Round(pos[0]), Math.Round(pos[1]), name, child));
		}

		FPrintln(m_File, "DONE");
		CloseFile(m_File);
		Print(string.Format("[ZTZ_Sampler] done: %1 buildings, %2 names. Shutting down.", m_Buildings, count));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().RequestExit, 2000, false, 0);
	}
}

ref ZTZ_TerrainSampler g_ZTZ_Sampler;

void main()
{
	Hive ce = CreateHive();
	if (ce)
		ce.InitOffline();

	g_ZTZ_Sampler = new ZTZ_TerrainSampler();
	GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(g_ZTZ_Sampler.Start, 5000, false);
}

Mission CreateCustomMission(string path)
{
	return new MissionServer();
}
