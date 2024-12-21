using System.Diagnostics;
using System.IO;

namespace AOC
{
    public class Maze
    {
        private Queue<(int x, int y)> frontier = new Queue<(int x, int y)>();
        private Dictionary<(int x, int y), (int x, int y)> cameFrom = new Dictionary<(int x, int y), (int x, int y)>();
        private (int x, int y) start;
        private (int x, int y) end;
        private int sizeX;
        private int sizeY;
        //private bool[,] map; // More efficient alternative using 2D arrays?
        private List<List<char>> map;
        private bool debugmode;
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
        public void Print()
        {
            Console.WriteLine("Printing map to screen...");
            for (int y = 0; y < sizeY; y++)
            {
                string line = new string(map[y].ToArray<char>());
                Console.WriteLine(line);
            }
        }
        private char GetChar(int x, int y)
        {
            return map[y][x];
        }
        private List<(int x, int y)> GetNeighbours((int x, int y) pos)
        {
            List<(int x, int y)> result = new List<(int x, int y)>();
            List<(int x, int y)> coords = new List<(int x, int y)>{
                (x: pos.x  , y: pos.y+1),
                (x: pos.x  , y: pos.y-1),
                (x: pos.x+1, y: pos.y  ),
                (x: pos.x-1, y: pos.y  ),
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
            frontier.Enqueue(start);
            cameFrom.Add(start, start);
            // Loop over all squares f in frontier, finding neighbours until
            // none are left, logging if we haven't visited them already
            while (frontier.Count != 0)
            {
                if (debugmode) { Console.WriteLine("Getting neighbours..."); }
                var f = frontier.Dequeue();
                var nbs = GetNeighbours(f);
                if (debugmode) { Console.WriteLine("Got {0} neighbours for {1},{2}", nbs.Count, f.x, f.y); }
                foreach (var n in nbs)
                {
                    if (cameFrom.ContainsKey(n))
                    {
                        // Been here already
                        if (debugmode) { Console.WriteLine("Already mapped square {0},{1}", n.x, n.y); }
                    }
                    else
                    {
                        if (debugmode) { Console.WriteLine("Adding square {0},{1} to frontier", n.x, n.y); }
                        frontier.Enqueue(n);
                        if (debugmode) { Console.WriteLine("Adding square {0},{1} to cameFrom, ref. {2},{3}", n.x, n.y, f.x, f.y); }
                        cameFrom.Add(n, f);
                    }
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

