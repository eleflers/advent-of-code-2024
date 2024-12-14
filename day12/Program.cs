using System.Diagnostics;
using System.IO;
using System.Runtime.CompilerServices;

namespace AOC
{
    public class GardenMap
    {
        private List<List<char>> map = new List<List<char>>();
        private List<Region> regions = new List<Region>();
        private Dictionary<int,HashSet<int>> mappedCoords = new Dictionary<int,HashSet<int>>();
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
        public long FenceCostP2
        {
            get => regions.Sum(x => x.FenceCostP2);
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
                mappedCoords.Add(x, new HashSet<int>(){ y });
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
                        var coord = r.WalkRegion();
                        foreach (var c in coord)
                        {
                            AddMappedCoord(c.x, c.y);
                        }
                    }
                    
                }
            }
        }
    }
    public enum Direction 
    {
        North,
        South,
        East,
        West
    }
    public class FenceSide
    {
        // If east or west facing fence, key is x pos. value is y
        private Dictionary<int, SortedSet<int>> lines = new Dictionary<int, SortedSet<int>>();
        private bool isEastOrWestFacing;
        public int LineCount
        {
            get
            {
                // Coord 1 is x/y and coord 2 is y/x
                // If same coord 1 and coord 2 is adjacent then the fence is part of the same line
                int separateLines = 0;
                foreach (var l in lines.Values)
                {
                    // Sorted set of coord 2, 'l'
                    // How many contiguous runs are there? Add that many onto total
                    int prev = l.First();
                    // Starts with at least 1
                    separateLines++;
                    if (l.Count > 1)
                    {
                        // Loop remainder checking for gaps, any gap is +1 to the total
                        for (int i = 1; i < l.Count; i++)
                        {
                            if (l.ElementAt(i) - prev > 1)
                            {
                                // Gap, so add 1
                                separateLines++;
                            }
                            prev = l.ElementAt(i);
                        }
                    }
                }
                return separateLines;
            }
        }
        public void AddFence(int x, int y)
        {
            if (isEastOrWestFacing)
            {
                // Key is x coord.
                if (lines.ContainsKey(x))
                {
                    lines[x].Add(y);
                }
                else
                {
                    lines.Add(x, new SortedSet<int>(){ y });
                }
            }
            else
            {
                // Key is y coord.
                if (lines.ContainsKey(y))
                {
                    lines[y].Add(x);
                }
                else
                {
                    lines.Add(y, new SortedSet<int>(){ x });
                }
            }
        }
        public FenceSide(bool isEastOrWestFacing)
        {
            this.isEastOrWestFacing = isEastOrWestFacing;
        }
    }
    public class Fencing
    {
        private FenceSide northFences = new FenceSide(false);
        private FenceSide southFences = new FenceSide(false);
        private FenceSide eastFences = new FenceSide(true);
        private FenceSide westFences = new FenceSide(true);
        public int StraightSideCount
        {
            get => northFences.LineCount + southFences.LineCount + eastFences.LineCount + westFences.LineCount;
        }
        public Fencing()
        {
        }
        public void AddFence(int x, int y, Direction d)
        {
            switch (d)
            {
                case Direction.North:
                {
                    northFences.AddFence(x, y);
                    break;
                }
                case Direction.South:
                {
                    southFences.AddFence(x, y);
                    break;
                }
                case Direction.East:
                {
                    eastFences.AddFence(x, y);
                    break;
                }
                case Direction.West:
                {
                    westFences.AddFence(x, y);
                    break;
                }
                default: break;
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
        private Fencing fences = new Fencing();
        private Dictionary<int,HashSet<int>> coords = new Dictionary<int, HashSet<int>>();
        public long FenceCost
        {
            get => area * perimeter;
        }
        public long FenceCostP2
        {
            // Part 2, fence cost is now area * number of distinct straight sides
            get => area * fences.StraightSideCount;
        }
        public Region(int startX, int startY, GardenMap garden)
        {
            this.startX = startX;
            this.startY = startY;
            this.garden = garden;
            this.plant = garden.GetPlantAtCoord(startX, startY);
            //Console.WriteLine("Made region for plant {0} at x:{1} y:{2}", plant, startX, startY);
        }
        public List<(int x, int y)> WalkRegion()
        {
            //Console.WriteLine("Begin walking region for plant {0} at x:{1} y:{2}", plant, startX, startY);
            // Method is recursive
            WalkRegion(startX, startY);
            //Console.WriteLine("Done. Area = {0} Perimeter = {1}", area, perimeter);
            // Convert to a named tuple to output coords
            List<(int x, int y)> c = new List<(int x, int y)>();
            foreach (var x in coords.Keys)
            {
                foreach (var y in coords[x])
                {
                    c.Add((x, y));
                }
            }
            return c;
        }
        private void WalkRegion(int x, int y)
        {
            if (garden.CountPlantAtCoord(x, y, plant) == 1)
            {
                //Console.WriteLine("Walk found x:{0} y:{1}", x, y);
                List<(int x, int y)> neighbouringCoords = new List<(int x, int y)>()
                {
                    (x: x - 1, y: y    ),
                    (x: x + 1, y: y    ),
                    (x: x,     y: y - 1),
                    (x: x,     y: y + 1),
                };

                // Count what we have around this position
                area++;
                int surroundingFences = 4 - garden.CountPlantAtCoord(neighbouringCoords, plant);
                perimeter += surroundingFences;
                // Find out where the fences were if any
                if (surroundingFences > 0 && garden.CountPlantAtCoord(x  , y+1, plant) != 1)
                {
                    // North
                    fences.AddFence(x  , y+1, Direction.North);
                    surroundingFences--;
                }
                if (surroundingFences > 0 && garden.CountPlantAtCoord(x  , y-1, plant) != 1)
                {
                    // South
                    fences.AddFence(x  , y-1, Direction.South);
                    surroundingFences--;
                }
                if (surroundingFences > 0 && garden.CountPlantAtCoord(x+1, y  , plant) != 1)
                {
                    // East
                    fences.AddFence(x+1, y  , Direction.East);
                    surroundingFences--;
                }
                if (surroundingFences > 0 && garden.CountPlantAtCoord(x-1, y  , plant) != 1)
                {
                    // West
                    fences.AddFence(x-1, y  , Direction.West);
                    surroundingFences--;
                }

                // Log where we've been
                AddMappedCoord(x, y);

                // Recursively go in all 4 directions, except those we've already visited
                foreach (var c in neighbouringCoords)
                {
                    if (!IsMappedCoord(c.x, c.y))
                    {
                        WalkRegion(c.x, c.y);
                    }
                }
            }
        }
        // Log a coord. as having been mapped already
        private void AddMappedCoord(int x, int y)
        {
            if (!coords.ContainsKey(x))
            {
                coords.Add(x, new HashSet<int>(){ y });
            }
            else
            {
                coords[x].Add(y);
            }
        }
        private bool IsMappedCoord(int x, int y)
        {
            return coords.ContainsKey(x) && coords[x].Contains(y);
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
            long sumP2 = garden.FenceCostP2;

            // Output
            // P2 1365986 too high
            Console.WriteLine("--");
            Console.WriteLine("Fence Cost (P1) = £{0}.00", sum);
            Console.WriteLine("Fence Cost (P2) = £{0}.00", sumP2);
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

