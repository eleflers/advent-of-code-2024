using System.Diagnostics;
using System.IO;

namespace AOC
{
    public enum Direction : int
    {
        North = 0,
        East = 1,
        South = 2,
        West = 3,
    }
    public class Maze
    {
        private PriorityQueue<(int x, int y), int> frontier = new PriorityQueue<(int x, int y), int>();
        private Dictionary<(int x, int y), (int x, int y)> cameFrom = new Dictionary<(int x, int y), (int x, int y)>();
        private Dictionary<(int x, int y), (int cost, Direction d)> costSoFar = new Dictionary<(int x, int y), (int cost, Direction d)>();
        private (int x, int y) start;
        private (int x, int y) end;
        private int sizeX;
        private int sizeY;
        //private bool[,] map; // More efficient alternative using 2D arrays?
        private List<List<char>> map;
        private bool debugmode;
        // Reindeer always starts facing East
        private Direction startingDirection = Direction.East;
        private const int moveCost = 1;
        private const int turnCost = 1000;
        public int Cost
        {
            get
            {
                if (costSoFar.Count == 0)
                {
                    Map();
                }
                return costSoFar[end].cost;
            }
        }
        public Maze(List<string> input, (int x, int y) start, (int x, int y) end, bool debugmode)
        {
            this.sizeX = input.First().Length;
            this.sizeY = input.Count;
            this.start = start;
            this.end = end;
            this.debugmode = debugmode;
            Console.Write("Parsing maze... ");
            this.map = new List<List<char>>();
            for (int y = 0; y < sizeY; y++)
            {
                string line = input[y];
                this.map.Add(line.ToList<char>());
            }
            Console.WriteLine("Done");
        }
        public void Print(bool markMappedLocations = false)
        {
            Console.WriteLine("Printing map to screen...");
            for (int y = 0; y < sizeY; y++)
            {
                for (int x = 0; x < sizeX; x++)
                {
                    if (markMappedLocations && cameFrom.ContainsKey((x, y)))
                    {
                        Console.Write("/");
                    }
                    else
                    {
                        Console.Write(map[y][x]);
                    }
                }
                Console.WriteLine();
            }
        }
        private char GetChar(int x, int y)
        {
            return map[y][x];
        }
        // Return list of viable neighbours + the cost required to get there (turncost + stepcost)
        private List<(int x, int y, int cost, Direction d)> GetNeighbours((int x, int y) pos, Direction dir)
        {
            // Figure out min. turns req'd, can turn in either direction
            // Get clockwise first, counterclockwise is 4-clockwise mod 4
            int clockwiseTurnsForNorth = ((int)Direction.North - (int)dir + 4) % 4;
            int clockwiseTurnsForEast = (clockwiseTurnsForNorth + 1) % 4;
            int clockwiseTurnsForSouth = (clockwiseTurnsForNorth + 2) % 4;
            int clockwiseTurnsForWest = (clockwiseTurnsForNorth + 3) % 4;
            // Calculate costs
            int turnCostNorth = 1000 * Math.Min(clockwiseTurnsForNorth, (4 - clockwiseTurnsForNorth) % 4);
            int turnCostEast = 1000 * Math.Min(clockwiseTurnsForEast, (4 - clockwiseTurnsForEast) % 4);
            int turnCostSouth = 1000 * Math.Min(clockwiseTurnsForSouth, (4 - clockwiseTurnsForSouth) % 4);
            int turnCostWest = 1000 * Math.Min(clockwiseTurnsForWest, (4 - clockwiseTurnsForWest) % 4);

            // List to store viable results (that aren't out of bounds or walls)
            List<(int x, int y, int cost, Direction d)> result = new List<(int x, int y, int cost, Direction d)>();
            // All possible directions to check, North -> East -> South -> West
            // Remember y is +ve in a South direction due to line 0 starting at the top of the file
            List<(int x, int y, int cost, Direction d)> coords = new List<(int x, int y, int cost, Direction d)>{
                (x: pos.x  , y: pos.y-1, cost: 1 + turnCostNorth, d: Direction.North),
                (x: pos.x+1, y: pos.y  , cost: 1 + turnCostEast, d: Direction.East),
                (x: pos.x  , y: pos.y+1, cost: 1 + turnCostSouth, d: Direction.South),
                (x: pos.x-1, y: pos.y  , cost: 1 + turnCostWest, d: Direction.West),
            };
            foreach (var n in coords)
            {
                if (n.x >= 0 && n.x < sizeX)
                {
                    if (n.y >= 0 && n.y < sizeY)
                    {
                        // In bounds
                        char c = GetChar(n.x, n.y);
                        if (c != '#')
                        {
                            // Anything other than a wall is good
                            result.Add(n);
                        }
                    }
                }
            }
            return result;
        }
        public void Map()
        {
            Console.WriteLine("Mapping maze...");
            frontier.Enqueue(start, 0);
            cameFrom.Add(start, start);
            costSoFar.Add(start, (0, startingDirection));

            // Loop over all squares f in frontier, finding neighbours until
            // none are left, logging if we haven't visited them already
            // This is not just a BFS, we need to take into account the cost of the neighbour
            // since some require turns, so take this into account when logging previously
            // seen squares as we may find a better route to one
            // TODO: Can we still exit early, since it is a frontier square and not a neighbour the priority
            // queue should ensure we already fully mapped the best way there by the time it is leaving
            // the frontier queue
            while (frontier.Count != 0)
            {
                if (debugmode) { Console.WriteLine("Getting neighbours..."); }
                var f = frontier.Dequeue();
                if (f == end)
                {
                    // Found destination, quit early
                    Console.WriteLine("Reached destination at {0},{1}, ending search", f.x, f.y);
                    break;
                }
                var nbs = GetNeighbours(f, costSoFar[f].d);
                if (debugmode) { Console.WriteLine("Got {0} neighbours for {1},{2}", nbs.Count, f.x, f.y); }
                foreach (var n in nbs)
                {
                    (int x, int y) key = (n.x, n.y);
                    (int cost, Direction d) newCostDir = (costSoFar[f].cost + n.cost, n.d);
                    if (costSoFar.ContainsKey(key) && costSoFar[key].cost <= newCostDir.cost)
                    {
                        // Been here already, and cost is no better now
                        if (debugmode) { Console.WriteLine("Already mapped square {0},{1}", n.x, n.y); }
                    }
                    else if (costSoFar.ContainsKey(key))
                    {
                        // Update an existing record with a better cost
                        if (debugmode) { Console.WriteLine("Found a better cost for square {0},{1}", n.x, n.y); }
                        costSoFar[key] = newCostDir;
                        cameFrom[key] = f;
                        // TODO: Don't need to queue it to frontier again since it was already visited?
                    }
                    else
                    {
                        // Not visited here before
                        if (debugmode) { Console.WriteLine("Adding square {0},{1} to frontier", n.x, n.y); }
                        frontier.Enqueue(key, newCostDir.cost);
                        if (debugmode) { Console.WriteLine("Adding square {0},{1} to cameFrom, ref. {2},{3}", n.x, n.y, f.x, f.y); }
                        costSoFar.Add(key, newCostDir);
                        cameFrom.Add(key, f);
                    }
                }
                if (debugmode)
                {
                    Console.Clear();
                    Print(markMappedLocations: true);
                    Thread.Sleep(250);
                }
            }
        }
    }
    public class Program
    {
        public static int Main(string[] args)
        {
            Console.WriteLine("--\nAoC Day 16\n--");
            // For testing
            int debuglimit = -1;
            bool debugmode = false;

            // User args
            List<string> argv = args.ToList();
            string filename = "input";
            if (argv.Count == 0)
            {
                Console.WriteLine("Assume default input file '{0}'", filename);
            }
            else if (argv.Count > 0)
            {
                filename = argv[0];
                Console.WriteLine("Taking CLI input file name '{0}'", filename);
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

            // Read file
            String? line;
            int lineNr = 0;
            List<string> lines = new List<string>();
            (int x, int y) start = (0, 0);
            (int x, int y) end = (0, 0);
            using (var streamReader = new StreamReader(filename))
            {
                while ((line = streamReader.ReadLine()) != null && (!debugmode || debuglimit == -1 || lineNr < debuglimit))
                {
                    if (debugmode)
                    {
                        Console.WriteLine(line);
                    }

                    // Input map is just a maze, walls with '#' and open spaces with '.'
                    // Starting pos. 'S', ending pos. 'E'
                    if (!string.IsNullOrWhiteSpace(line))
                    {
                        lines.Add(line);
                    }
                    int pos = line.IndexOf('S');
                    if (pos != -1)
                    {
                        Console.WriteLine("Found 'S' at line {0} column {1}", lineNr, pos);
                        start = (pos, lineNr);
                    }
                    pos = line.IndexOf('E');
                    if (pos != -1)
                    {
                        Console.WriteLine("Found 'E' at line {0} column {1}", lineNr, pos);
                        end = (pos, lineNr);
                    }

                    lineNr++;
                }
            }

            // Processing
            Maze m = new Maze(lines, start, end, debugmode);
            if (debugmode)
            {
                m.Print();
            }
            m.Map();
            sum = m.Cost;

            // Output
            Console.WriteLine("--");
            Console.WriteLine("Sum = {0}", sum);
            if (sum == 7036 || sum == 11048)
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

