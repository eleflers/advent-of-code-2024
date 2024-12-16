using System.Diagnostics;
using System.IO;

namespace AOC
{
    public enum Direction
    {
        Up,
        Right,
        Down,
        Left
    }
    public interface IWarehouseItem
    {
        public int x { get; set; }
        public int y { get; set; }
        public bool TryMove(Direction d);
    }
    public class Wall : IWarehouseItem
    {
        public int x { get; set; }
        public int y { get; set; }
        public Wall(int x, int y)
        {
            this.x = x;
            this.y = y;
        }
        public bool TryMove(Direction d)
        {
            return false;
        }
    }
    public class Box : IWarehouseItem
    {
        public int x { get; set; }
        public int y { get; set; }
        private Warehouse w;
        public int GPScoords
        {
            get => x + y*100;
        }
        public Box(int x, int y, Warehouse w)
        {
            this.x = x;
            this.y = y;
            this.w = w;
        }
        public bool TryMove(Direction d)
        {
            int newX;
            int newY;
            // Parse direction
            switch (d)
            {
                case Direction.Up:
                    newX = x;
                    newY = y - 1;
                    break;
                case Direction.Down:
                    newX = x;
                    newY = y + 1;
                    break;
                case Direction.Left:
                    newX = x - 1;
                    newY = y;
                    break;
                case Direction.Right:
                    newX = x + 1;
                    newY = y;
                    break;
                default:
                    // Keep compiler happy
                    throw new InvalidDataException("Invalid Direction d");
            }
            // Test to see if we're moving into an empty space
            if (w.SpotIsEmpty(newX, newY))
            {
                Move(newX, newY);
                return true;
            }
            else
            {
                // Have to try and push whatever is blocking us
                IWarehouseItem blocker = w.GetItem(newX, newY);
                if (blocker.TryMove(d))
                {
                    // Success!
                    Move(newX, newY);
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
        private void Move(int newX, int newY)
        {
            int oldX = x;
            int oldY = y;
            x = newX;
            y = newY;
            w.MoveItem(oldX, oldY, newX, newY);
        }
    }
    public class Warehouse
    {
        private Dictionary<int,Dictionary<int,IWarehouseItem>> items = new Dictionary<int, Dictionary<int, IWarehouseItem>>();
        private int maxX = 0;
        private int maxY = 0;
        // Keep a separate list of the boxes in 'items', to do operations on them later
        private List<Box> boxes = new List<Box>();
        public int SumBoxGPScoords
        {
            get => boxes.Sum(x => x.GPScoords);
        }
        public Warehouse()
        {
        }
        public void PlaceItem(IWarehouseItem i)
        {
            if (!items.ContainsKey(i.y))
            {
                items.Add(i.y, new Dictionary<int, IWarehouseItem>());
            }
            items[i.y].Add(i.x, i);
            // Keep track of size
            if (i.x > maxX)
            {
                maxX = i.x;
            }
            if (i.y > maxY)
            {
                maxY = i.y;
            }
        }
        public void AddBox(Box b)
        {
            boxes.Add(b);
        }
        public IWarehouseItem GetItem(int x, int y)
        {
            return items[y][x];
        }
        public bool SpotIsEmpty(int x, int y)
        {
            if (items.ContainsKey(y))
            {
                if (items[y].ContainsKey(x))
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            else
            {
                throw new InvalidDataException("Dictionary is missing a row entry for y");
            }
        }
        public void MoveItem(int oldX, int oldY, int newX, int newY)
        {
            IWarehouseItem i = GetItem(oldX, oldY);
            items[oldY].Remove(oldX);
            items[newY].Add(newX, i);
        }
        public void DrawWarehouse(Robot r)
        {
            // Draw a pretty picture for debugging
            List<char[]> pic = new List<char[]>();
            for (int i = 0; i < maxY+1; i++)
            {
                pic.Add(new string('.', maxX+1).ToCharArray());
            }
            foreach (var y in items.Keys)
            {
                foreach (var obj in items[y])
                {
                    pic[y][obj.Key] = '#';
                }
            }
            foreach (var b in boxes)
            {
                pic[b.y][b.x] = 'O';
            }
            pic[r.y][r.x] = '@';
            Console.WriteLine("Warehouse after {0} move instructions:", r.MoveAttempts);
            foreach (var line in pic)
            {
                Console.WriteLine(line);
            }
            Console.WriteLine();
        }
    }
    public class Robot : IWarehouseItem
    {
        public int x { get; set; }
        public int y { get; set; }
        private Warehouse w;
        public int MoveAttempts = 0;
        public Robot(int x, int y, Warehouse warehouse)
        {
            this.x = x;
            this.y = y;
            this.w = warehouse;
        }
        public void ExecuteMoves(Queue<Direction> instr, int limit = -1, bool draw = false)
        {
            int i = 0;
            while (instr.Count != 0 && i++ != limit)
            {
                if (draw || i % 200 == 0 || instr.Count == 1)
                {
                    Console.WriteLine("Executing instruction {0}/{1}...", i, i+instr.Count-1);
                }
                TryMove(instr.Dequeue());
                if (draw || i % 1000 == 0 || instr.Count == 0)
                {
                    w.DrawWarehouse(this);
                }
            }
        }
        public bool TryMove(Direction d)
        {
            int newX;
            int newY;
            MoveAttempts++;
            // Parse direction
            switch (d)
            {
                case Direction.Up:
                    newX = x;
                    newY = y - 1;
                    break;
                case Direction.Down:
                    newX = x;
                    newY = y + 1;
                    break;
                case Direction.Left:
                    newX = x - 1;
                    newY = y;
                    break;
                case Direction.Right:
                    newX = x + 1;
                    newY = y;
                    break;
                default:
                    // Keep compiler happy
                    throw new InvalidDataException("Invalid Direction d");
            }
            // Test to see if we're moving into an empty space
            if (w.SpotIsEmpty(newX, newY))
            {
                Move(newX, newY);
                return true;
            }
            else
            {
                // Have to try and push whatever is blocking us
                IWarehouseItem blocker = w.GetItem(newX, newY);
                if (blocker.TryMove(d))
                {
                    // Success!
                    Move(newX, newY);
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
        private void Move(int newX, int newY)
        {
            x = newX;
            y = newY;
        }
    }
    public class Program
    {
        public static int Main(string[] args)
        {
            Console.WriteLine("--\nAoC Day 15\n--");
            // For testing
            int debuglimit = 21;
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
            Queue<Direction> instructions = new Queue<Direction>();
            Robot? robot = null;
            Warehouse warehouse = new Warehouse();
            int section = 1;
            using (var streamReader = new StreamReader(filename))
            {
                while ((line = streamReader.ReadLine()) != null && (!debugmode || lineNr < debuglimit))
                {
                    if (debugmode)
                    {
                        Console.WriteLine(line);
                    }

                    // Parse input
                    if (string.IsNullOrWhiteSpace(line))
                    {
                        // There is a blank line between map and robot instructions
                        section = 2;
                    }
                    else if (section == 1)
                    {
                        // Section 1 - Warehouse map
                        // Search walls
                        int pos = line.IndexOf("#");
                        while (pos != -1)
                        {
                            Wall i = new Wall(pos, lineNr);
                            warehouse.PlaceItem(i);
                            pos = line.IndexOf("#", pos+1);
                        }
                        // Boxes
                        pos = line.IndexOf("O");
                        while (pos != -1)
                        {
                            Box i = new Box(pos, lineNr, warehouse);
                            warehouse.PlaceItem(i);
                            warehouse.AddBox(i);
                            pos = line.IndexOf("O", pos+1);
                        }
                        // See if the robot is on this line too
                        pos = line.IndexOf("@");
                        if (pos != -1)
                        {
                            robot = new Robot(pos, lineNr, warehouse);
                        }
                    }
                    else
                    {
                        // Section 2 - Robot move instructions
                        // One of <, v, >, ^
                        // Be careful with directions, problem states y = 0 is at the top of the image,
                        // so v will be y=y+1
                        foreach (char i in line)
                        {
                            switch (i)
                            {
                                case '^':
                                    instructions.Enqueue(Direction.Up);
                                    break;
                                case '>':
                                    instructions.Enqueue(Direction.Right);
                                    break;
                                case 'v':
                                    instructions.Enqueue(Direction.Down);
                                    break;
                                case '<':
                                    instructions.Enqueue(Direction.Left);
                                    break;
                                default:
                                    Console.WriteLine("Unexpected char '{0}' in robot instructions line {1}", i, lineNr);
                                    throw new InvalidDataException("Unexpected char in robot instructions line ");
                            }
                        }
                    }

                    lineNr++;
                }
            }

            // Processing
            if (robot == null)
            {
                throw new InvalidDataException("We didn't get a robot!");
            }
            if (debugmode)
            {
                Console.WriteLine("Robot is at x,y [{0},{1}], line {2} col {3}", robot.x, robot.y, robot.y+1, robot.x+1);
                Console.WriteLine("Parsed {0} robot instructions", instructions.Count);
            }
            // Pass in instructions
            robot.ExecuteMoves(instructions);
            sum = warehouse.SumBoxGPScoords;

            // Output
            Console.WriteLine("--");
            Console.WriteLine("Sum = {0}", sum);
            if (sum == 10092 || sum == 2028)
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

