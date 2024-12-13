using System.Diagnostics;
using System.IO;

namespace AOC
{
    public class GardenMap
    {
        private List<List<char>> map = new List<List<char>>();
        public Dictionary<char,long> PlantAreas = new Dictionary<char,long>();
        public Dictionary<char,long> PlantPerimeters = new Dictionary<char,long>();
        public long FenceCost
        {
            get => PlantAreas.Keys.Sum(x => PlantAreas[x] * PlantPerimeters[x]);
        }
        public int sizeX
        {
            get => map[0].Count;
        }
        public int sizeY
        {
            get => map.Count;
        }
        public GardenMap()
        {
        }
        public void AddRow(string line)
        {
            map.Add(line.ToList<char>());
        }
        public char GetPlantAtCoord(int x, int y)
        {
            return map[y][x];
        }
        public int CountPlantAtCoord(int x, int y, char plant)
        {
            // Boundary Check
            if (x < 0 || x >= sizeX || y < 0 || y >= sizeY)
            {
                return 0;
            }
            return plant == GetPlantAtCoord(x, y) ? 1 : 0;
        }
        public int CountPlantAtCoord(List<(int x, int y)> coords, char plant)
        {
            int res = 0;
            foreach (var c in coords)
            {
                res += CountPlantAtCoord(c.x, c.y, plant);
            }
            return res;
        }
        public void ScanGarden()
        {
            for (int i = 0; i < sizeX; i++)
            {
                for (int j = 0; j < sizeY; j++)
                {
                    ScanGarden(i, j);
                }
            }
        }
        public void ScanGarden(int x, int y)
        {
            char plant = GetPlantAtCoord(x, y);
            // How many squares do we border that contain the same plant
            List<(int x, int y)> coords = new List<(int x, int y)>()
            {
                (x: x - 1, y: y    ),
                (x: x + 1, y: y    ),
                (x: x,     y: y - 1),
                (x: x,     y: y + 1),
            };
            int sameRegionBorders = CountPlantAtCoord(coords, plant);

            // Update dictionaries
            // Area = how many squares of this plant type
            if (PlantAreas.ContainsKey(plant))
            {
                PlantAreas[plant] += 1;
            }
            else
            {
                PlantAreas.Add(plant, 1);
            }
            // Perimeter = how many cells do we border that AREN'T the same as us
            // e.g. all 4 shared = 4 - 4 = 0, 1 shared = 4-1 = 3
            if (PlantPerimeters.ContainsKey(plant))
            {
                PlantPerimeters[plant] += 4-sameRegionBorders;
            }
            else
            {
                PlantPerimeters.Add(plant, 4-sameRegionBorders);
            }
        }
    }
    public class Region
    {
        private int startX;
        private int startY;
        private char plant;
        private int area = 0;
        private int perimeter = 0;
        public Region(int startingX, int startingY, char plant)
        {
            this.startX = startingX;
            this.startY = startingY;
        }
    }
    public class Program
    {
        public static int Main(string[] args)
        {
            Console.WriteLine("--\nAoC Day 12\n--");
            // For testing
            int debuglimit = 10;
            bool debugmode = false;

            // User args
            List<string> argv = args.ToList();
            string filename = "input";
            if (argv.Count == 0)
            {
                Console.WriteLine("Assume default input file '{0}", filename);
            }
            else if (argv.Count > 0)
            {
                filename = argv[0];
                Console.WriteLine("Taking CLI input file name {0}'", filename);
            }
            if (argv.Count > 1)
            {
                Console.WriteLine("Reading 2nd input param\n    -d / --debug for debug printing");
                if (argv[1] == "-d" || argv[1] == "--debug")
                {
                    debugmode = true;
                }
                else
                {
                    debugmode = false;
                }
            }
            if (debugmode)
            {
                Console.WriteLine("--\nDEBUG MODE : ON\nLINE LIMIT : {0}", debuglimit);
            }

            // Variables for output
            long sum = 0;
            GardenMap garden = new GardenMap();

            // Read file
            String? line;
            int lineNr = 0;
            using (var streamReader = new StreamReader(filename))
            {
                while ((line = streamReader.ReadLine()) != null && (!debugmode || lineNr < debuglimit))
                {
                    if (debugmode) {
                        Console.WriteLine(line);
                    }

                    garden.AddRow(line);

                    lineNr++;
                }
            }

            // Processing
            if (debugmode)
            {
            }
            Console.WriteLine("Read in garden size {0}, {0}", garden.sizeX, garden.sizeY);
            // Scan garden
            garden.ScanGarden();
            sum = garden.FenceCost;

            // Output
            Console.WriteLine("--");
            Console.WriteLine("Fence Cost (P1) = £{0}.00", sum);
            if (sum == 140 || sum == 772 || sum == 1930)
            {
                Console.WriteLine("Answer matches example expected answer");
            }
            else
            {
                Console.WriteLine("Answer does not match example expected answer");
            }

            Console.WriteLine("--\nEnd.");
            return 0;
        }
    }
}

