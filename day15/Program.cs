using System.Diagnostics;
using System.IO;
using System.Threading;

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
        public bool TryMove(Direction d, bool dryrun = false);
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
        public bool TryMove(Direction d, bool dryrun = false)
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
        public bool TryMove(Direction d, bool dryrun = false)
        {
            int newX;
            int newY;
            // For P2 horizontal moves we have to probe further ahead in X dir.
            // For P2 vertical moves we have to probe x+1 also as box is now 2 units wide
            // Store these in var.
            int probeX;
            // Parse direction
            switch (d)
            {
                case Direction.Up:
                    newX = x;
                    newY = y - 1;
                    probeX = x + 1;
                    break;
                case Direction.Down:
                    newX = x;
                    newY = y + 1;
                    probeX = x + 1;
                    break;
                case Direction.Left:
                    newX = x - 1;
                    newY = y;
                    probeX = x - 1; // Don't need to peek + 1 ahead, because box x-coords are tracked from left hand side
                    break;
                case Direction.Right:
                    newX = x + 1;
                    newY = y;
                    probeX = x + 2; // Peek 1 further ahead than we move
                    break;
                default:
                    // Keep compiler happy
                    throw new InvalidDataException("Invalid Direction d");
            }
            // Test to see if we're moving into an empty space
            // Part 2 we have to peek 2 spaces ahead not 1, but we still only move 1
            if (!w.Part2 && w.SpotIsEmpty(newX, newY))
            {
                if (!dryrun)
                {
                    Move(newX, newY);
                }
                return true;
            }
            else if (w.Part2 && (d == Direction.Up || d == Direction.Down)
                    && w.SpotIsEmpty(newX, newY)
                    && w.SpotIsEmpty(probeX, newY))
            {
                // Up/down moves need to check x+1 also as box is twice as wide
                if (!dryrun)
                {
                    Move(newX, newY);
                }
                return true;
            }
            else if (w.Part2 && (d == Direction.Left || d == Direction.Right)
                    && w.SpotIsEmpty(probeX, newY))
            {
                // Left/right moves need to peek 2 ahead instead of 1, as box is twice as wide
                // But still moves only 1 space
                if (!dryrun)
                {
                    Move(newX, newY);
                }
                return true;
            }
            else
            {
                // Have to try and push whatever is blocking us
                // For Part 2 remember to hand down the dryrun parameter, because we could just be probing to see
                // if a move is possible. In that case we want to ask the same of blockers not move them
                IWarehouseItem blocker;
                if (!w.Part2)
                {
                    blocker = w.GetItem(newX, newY);
                    if (blocker.TryMove(d, dryrun))
                    {
                        // Success!
                        if (!dryrun)
                        {
                            Move(newX, newY);
                        }
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else if (d == Direction.Left || d == Direction.Right)
                {
                    // Part 2 left and right
                    blocker = w.GetItem(probeX, newY);
                    if (blocker.TryMove(d, dryrun))
                    {
                        // Success!
                        if (!dryrun)
                        {
                            Move(newX, newY);
                        }
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    // Part 2 up and down, for this we could have 1 or 2 different blockers
                    IWarehouseItem blocker2;
                    if (!w.SpotIsEmpty(newX, newY))
                    {
                        blocker = w.GetItem(newX, newY);
                        if (!w.SpotIsEmpty(probeX, newY))
                        {
                            blocker2 = w.GetItem(probeX, newY);
                            if (blocker.x != blocker2.x || blocker.y != blocker2.y)
                            {
                                // Have 2 blockers to deal with
                                // Must try both independently without executing the move until we know if it will work
                                if (blocker.TryMove(d, dryrun: true) && blocker2.TryMove(d, dryrun: true))
                                {
                                    // Move is possible, so do it (if this move is not itself also a dryrun!)
                                    if (!dryrun)
                                    {
                                        blocker.TryMove(d);
                                        blocker2.TryMove(d);
                                        Move(newX, newY);
                                    }
                                    return true;
                                }
                                else
                                {
                                    // Move is not possible for 1 or more blockers
                                    return false;
                                }
                            }
                        }
                    }
                    else
                    {
                        blocker = w.GetItem(probeX, newY);
                    }
                    if (blocker.TryMove(d, dryrun))
                    {
                        // Success!
                        if (!dryrun)
                        {
                            Move(newX, newY);
                        }
                        return true;
                    }
                    else
                    {
                        return false;
                    }
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
        public bool Part2;
        // Keep a separate list of the boxes in 'items', to do operations on them later
        private List<Box> boxes = new List<Box>();
        public int SumBoxGPScoords
        {
            get => boxes.Sum(x => x.GPScoords);
        }
        public Warehouse(bool part2)
        {
            this.Part2 = part2;
        }
        public void PlaceItem(IWarehouseItem i, int xOffset = 0)
        {
            if (!items.ContainsKey(i.y))
            {
                items.Add(i.y, new Dictionary<int, IWarehouseItem>());
            }
            items[i.y].Add(i.x + xOffset, i);
            // Keep track of size
            if (i.x + xOffset > maxX)
            {
                maxX = i.x + xOffset;
            }
            if (i.y > maxY)
            {
                maxY = i.y;
            }
            // Part 2 place the same item in at x + 1 pos'n so we can hit either square
            // Make sure it's not recursive by checking for xOffset
            if (Part2 && xOffset == 0)
            {
                PlaceItem(i, xOffset: 1);
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
            if (Part2)
            {
                // Also x + 1 for P2, point to same obj, so either square can be hit
                items[oldY].Remove(oldX + 1);
                items[newY].Add(newX + 1, i);
            }
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
                if (Part2)
                {
                    pic[b.y][b.x] = '[';
                    pic[b.y][b.x+1] = ']';
                }
                else
                {
                    pic[b.y][b.x] = 'O';
                }
            }
            pic[r.y][r.x] = '@';
            Console.Clear();
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
                    Thread.Sleep(250);
                }
            }
        }
        public bool TryMove(Direction d, bool dryrun = false)
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
                if (!dryrun)
                {
                    Move(newX, newY);
                }
                return true;
            }
            else
            {
                // Have to try and push whatever is blocking us
                IWarehouseItem blocker = w.GetItem(newX, newY);
                if (blocker.TryMove(d, dryrun))
                {
                    // Success!
                    if (!dryrun)
                    {
                        Move(newX, newY);
                    }
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
            Robot? robotP2 = null;
            Warehouse warehouse = new Warehouse(false);
            Warehouse warehouseP2 = new Warehouse(true);
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
                            // P2
                            Wall iP2 = new Wall(2*pos, lineNr);
                            warehouseP2.PlaceItem(iP2);
                            // Next
                            pos = line.IndexOf("#", pos+1);
                        }
                        // Boxes
                        pos = line.IndexOf("O");
                        while (pos != -1)
                        {
                            // P1
                            Box i = new Box(pos, lineNr, warehouse);
                            warehouse.PlaceItem(i);
                            warehouse.AddBox(i);
                            // P2
                            Box iP2 = new Box(2*pos, lineNr, warehouseP2);
                            warehouseP2.PlaceItem(iP2);
                            warehouseP2.AddBox(iP2);
                            // Next
                            pos = line.IndexOf("O", pos+1);
                        }
                        // See if the robot is on this line too
                        pos = line.IndexOf("@");
                        if (pos != -1)
                        {
                            robot = new Robot(pos, lineNr, warehouse);
                            robotP2 = new Robot(2*pos, lineNr, warehouseP2);
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
            if (robot == null || robotP2 == null)
            {
                throw new InvalidDataException("We didn't get a robot!");
            }
            if (debugmode)
            {
                Console.WriteLine("Robot is at x,y [{0},{1}], line {2} col {3}", robot.x, robot.y, robot.y+1, robot.x+1);
                Console.WriteLine("Parsed {0} robot instructions", instructions.Count);
            }
            // Duplicate instructions
            Queue<Direction> instructionsP2 = new Queue<Direction>(instructions);
            // Pass in instructions P1
            robot.ExecuteMoves(instructions);
            sum = warehouse.SumBoxGPScoords;

            // P2
            robotP2.ExecuteMoves(instructionsP2);
            long sumP2 = warehouseP2.SumBoxGPScoords;

            // Output
            Console.WriteLine("--");
            Console.WriteLine("Sum (P1) = {0}", sum);
            Console.WriteLine("Sum (P2) = {0}", sumP2);
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

