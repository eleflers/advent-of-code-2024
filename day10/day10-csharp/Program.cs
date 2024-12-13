using System.Diagnostics;
using System.IO;

namespace AOC
{
    public class TopMap
    {
        public List<List<int>> topography = new List<List<int>>();
        public List<TrailHead> trailheads = new List<TrailHead>();
        public int SizeX
        {
            get => topography[0].Count; // col
        }
        public int SizeY
        {
            get => topography.Count; // rows
        }
        public bool IsHeight(int x, int y, int h)
        {
            if (x >= 0 && x < SizeX && y >= 0 && y < SizeY)
            {
                return (topography[y][x] == h);
            }
            else
            {
                return false;
            }
        }
    }

    public class TrailHead
    {
        public int startX;
        public int startY;
        private int score;
        private bool hasBeenWalked;
        private TopMap map;
        public TrailHead(int x, int y, TopMap map)
        {
            this.startX = x;
            this.startY = y;
            this.map = map;
            this.score = 0;
            this.hasBeenWalked = false;
        }
        public int GetTrailScore()
        {
            if (hasBeenWalked)
            {
                return score;
            }
            else
            {
                int startingHeight = 0;
                score = WalkTrail(startX, startY, startingHeight);
                hasBeenWalked = true;
                return score;
            }
        }
        private int WalkTrail(int x, int y, int h)
        {
            // Are we at pos. x y with height h?
            // If so return count of how many h+1 are reachable from this spot
            // We only care about directly left/right/up/down of us, no diagonal movement
            if (map.IsHeight(x, y, h))
            {
                if (h == 9)
                {
                    // We found a 9!
                    return 1;
                }
                else
                {
                    // Not at a 9 yet, so branch recursively on all possible paths
                    // They will terminate and return a 0 if the next required height is not at that spot
                    return
                        WalkTrail(x - 1, y    , h+1) +
                        WalkTrail(x + 1, y    , h+1) +
                        WalkTrail(x    , y - 1, h+1) +
                        WalkTrail(x    , y + 1, h+1);
                }
            }
            else
            {
                // Not a valid continuation of the path
                return 0;
            }
        }
    }

    public class Program
    {
        public static int Main(string[] args)
        {
            Console.WriteLine("--\nAoC Day 10\n--");
            // For testing
            int debuglimit = 10;
            int debug = 0;
            bool debugapply = false;

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
                    debugapply = true;
                }
                else
                {
                    debugapply = false;
                }
            }
            if (debugapply)
            {
                Console.WriteLine("--\nDEBUG MODE : ON\nLINE LIMIT : {0}", debuglimit);
            }

            // Variables for output
            long sum = 0;

            // Read file
            String? line;
            int lineNr = 0;
            TopMap tm = new TopMap();
            int invalidPoints = 0;
            using (var streamReader = new StreamReader(filename))
            {
                while (debug < debuglimit && (line = streamReader.ReadLine()) != null)
                {
                    debug++;
                    if (debugapply) {
                        Console.WriteLine(line);
                    }

                    List<int> row = new List<int>();
                    int val;
                    for (int i = 0; i < line.Length; i++)
                    {
                        if (int.TryParse(line.Substring(i,1), out val))
                        {
                            row.Add(val);
                            // If we found a 0, then we found a potential trailhead
                            if (val == 0)
                            {
                                tm.trailheads.Add(new TrailHead(i, lineNr, tm));
                            }
                        }
                        else
                        {
                            row.Add(-1);
                            invalidPoints++;
                        }
                    }

                    tm.topography.Add(row);
                    lineNr++;
                }
            }
            if (invalidPoints > 0)
            {
                Console.WriteLine("Warning: Found {0} points that were not valid digits 0-9, are you using test data?", invalidPoints);
            }

            // Processing
            Console.WriteLine("Found {0} trailheads to investigate", tm.trailheads.Count());
            Console.WriteLine("X Y bounds of map = [{0},{1}]", tm.SizeX, tm.SizeY);
            foreach (var th in tm.trailheads)
            {
                Console.WriteLine("Following trail at [{0},{1}]", th.startX, th.startY);
                sum += th.GetTrailScore();
                Console.WriteLine("It returned score: {0}", th.GetTrailScore());
            }

            // Output
            Console.WriteLine("--\n");
            Console.WriteLine("Sum = {0}", sum);
            if (sum == 36)
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

