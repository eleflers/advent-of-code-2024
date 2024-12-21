using System.Diagnostics;
using System.IO;

namespace AOC
{
    public class Point
    {
        int x;
        int y;
        public Point(int x, int y)
        {
            this.x = x;
            this.y = y;
        }
    }
    public class Maze
    {
        private Queue<Point> frontier = new Queue<Point>();
        private Dictionary<Point, Point> cameFrom = new Dictionary<Point, Point>();
        private Point start = null;
        private Point end = null;
        private int sizeX;
        private int sizeY;
        //private bool[,] map; // More efficient alternative using 2D arrays?
        private List<List<char>> map;
        public Maze(List<string> input, Point start, Point end)
        {
            this.sizeX = input.First().Length;
            this.sizeY = input.Count;
            this.start = start;
            this.end = end;
            Console.Write("Parsing map... ");
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
            Point start = null;
            Point end = null;
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
                        Console.WriteLine("Found 'S' at line {lineNr} column {pos}");
                        start = new Point(pos, lineNr);
                    }
                    pos = line.IndexOf('E');
                    if (pos != -1)
                    {
                        Console.WriteLine("Found 'E' at line {lineNr} column {pos}");
                        end = new Point(pos, lineNr);
                    }

                    lineNr++;
                }
            }

            // Processing
            Maze m = new Maze(lines, start, end);
            if (debugmode)
            {
                m.Print();
            }


            // Output
            Console.WriteLine("--");
            Console.WriteLine("Sum = {0}", sum);
            if (sum == 123)
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

