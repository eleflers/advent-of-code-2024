using System.Diagnostics;
using System.IO;

namespace AOC
{
    public class GardenMap
    {
        private List<List<char>> map = new List<List<char>>();
        private List<Region> regions = new List<Region>();
        private Dictionary<int,HashSet<int>> mappedCoords = new Dictionary<int, HashSet<int>>();
        public GardenMap()
        {
        }
        public int sizeX
        {
            get => map[0].Count;
        }
        public int sizeY
        {
            get => map.Count;
        }
        public long FenceCost
        {
            get => regions.Sum(x => x.FenceCost);
        }
        public void AddRow(string line)
        {
            map.Add(line.ToList<char>());
        }
        // Log a coord. as having been mapped already
        public void AddMappedCoord(int x, int y)
        {
            if (!mappedCoords.ContainsKey(x))
            {
                mappedCoords.Add(x, new HashSet<int>(y));
            }
            else
            {
                mappedCoords[x].Add(y);
            }
        }
        public bool IsMappedCoord(int x, int y)
        {
            return mappedCoords.ContainsKey(x) && mappedCoords[x].Contains(y);
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
                    // Scan and make regions
                    // Check coord already visited so we don't hit a region again
                    if (!IsMappedCoord(i, j))
                    {
                        Region r = new Region(i, j, this);
                        regions.Add(r);
                        r.WalkRegion();
                    }
                    
                }
            }
        }
    }
    public class Region
    {
        private GardenMap garden;
        private int startX;
        private int startY;
        private char plant;
        private int area = 0;
        private int perimeter = 0;
        public int FenceCost
        {
            get => area * perimeter;
        }
        public Region(int startX, int startY, GardenMap garden)
        {
            this.startX = startX;
            this.startY = startY;
            this.garden = garden;
            this.plant = garden.GetPlantAtCoord(startX, startY);
        }
        public void WalkRegion()
        {
            WalkRegion(startX, startY);
        }
        private void WalkRegion(int x, int y)
        {
            if (garden.CountPlantAtCoord(x, y, plant) == 1)
            {
                // Neighbouring coords
                List<(int x, int y)> coords = new List<(int x, int y)>()
                {
                    (x: x - 1, y: y    ),
                    (x: x + 1, y: y    ),
                    (x: x,     y: y - 1),
                    (x: x,     y: y + 1),
                };

                // Count what we have around this position
                area++;
                perimeter += 4 - garden.CountPlantAtCoord(coords, plant);

                // Log where we've been
                garden.AddMappedCoord(x, y);

                // Recursively go in all 4 directions, except those we've already visited
                foreach (var c in coords)
                {
                    if (!garden.IsMappedCoord(c.x, c.y))
                    {
                        WalkRegion(c.x, c.y);
                    }
                }
            }
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

