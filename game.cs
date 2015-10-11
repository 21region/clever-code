using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace Snake
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
     /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            var timer = new DispatcherTimer();
            timer.Tick += timer_Tick;

            /* Here user can change the speed of the snake. 
             * Possible speeds are FAST, MODERATE, SLOW and DAMNSLOW */
            timer.Interval = _moderate;
            timer.Start();

            KeyDown += OnButtonKeyDown;
            PaintSnake(_startingPoint);
            _currentPosition = _startingPoint;

            // Instantiate Food Objects
            for (int n = 0; n < 10; n++)
                PaintBonus(n);
        }

        /* This method is used to paint a frame of the snake´s body
         * each time it is called. */
        private void PaintSnake(Point currentPosition)
        {
            var newEllipse = new Ellipse
            {
                Fill = _snakeColor,
                Width = HeadSize,
                Height = HeadSize
            };

            Canvas.SetTop(newEllipse, currentPosition.Y);
            Canvas.SetLeft(newEllipse, currentPosition.X);

            var count = PaintCanvas.Children.Count;
            PaintCanvas.Children.Add(newEllipse);
            _snakePoints.Add(currentPosition);

            // Restrict the tail of the snake
            if (count > _length)
            {
                PaintCanvas.Children.RemoveAt(count - _length + 9);
                _snakePoints.RemoveAt(count - _length);
            }
        }

        private void PaintBonus(int index)
        {
            var newEllipse = new Ellipse
            {
                Fill = Brushes.Red,
                Width = HeadSize,
                Height = HeadSize
            };

            var bonusPoint = new Point(_random.Next(5, (int)(Height - newEllipse.Height)),
                                       _random.Next(5, (int)(Width - newEllipse.Width)));

            Canvas.SetTop(newEllipse, bonusPoint.Y);
            Canvas.SetLeft(newEllipse, bonusPoint.X);
            PaintCanvas.Children.Insert(index, newEllipse);
            _bonusPoints.Insert(index, bonusPoint);
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            // Expand the body of the snake to the direction of movement

            switch (_direction)
            {
                case (int)MovingDirection.Downwards:
                    _currentPosition.Y += 1;
                    PaintSnake(_currentPosition);
                    break;
                case (int)MovingDirection.Upwards:
                    _currentPosition.Y -= 1;
                    PaintSnake(_currentPosition);
                    break;
                case (int)MovingDirection.Toleft:
                    _currentPosition.X -= 1;
                    PaintSnake(_currentPosition);
                    break;
                case (int)MovingDirection.Toright:
                    _currentPosition.X += 1;
                    PaintSnake(_currentPosition);
                    break;
            }

            // Restrict to boundaries of the Canvas
            if ((_currentPosition.X < 5) || (_currentPosition.X > 620) ||
                (_currentPosition.Y < 5) || (_currentPosition.Y > 380))
                GameOver();

            // Hitting a bonus Point causes the lengthen-Snake Effect
            var n = 0;
            foreach (var point in _bonusPoints)
            {

                if ((Math.Abs(point.X - _currentPosition.X) < HeadSize) &&
                    (Math.Abs(point.Y - _currentPosition.Y) < HeadSize))
                {
                    _length += 10;
                    _score += 10;

                    // In the case of food consumption, erase the food object
                    // from the list of bonuses as well as from the canvas
                    _bonusPoints.RemoveAt(n);
                    PaintCanvas.Children.RemoveAt(n);
                    PaintBonus(n);
                    break;
                }
                n++;
            }

            // Restrict hits to body of Snake
            for (int q = 0; q < (_snakePoints.Count - HeadSize * 2); q++)
            {
                var point = new Point(_snakePoints[q].X, _snakePoints[q].Y);
                if ((Math.Abs(point.X - _currentPosition.X) < (HeadSize)) &&
                    (Math.Abs(point.Y - _currentPosition.Y) < (HeadSize)))
                {
                    GameOver();
                    break;
                }
            }
        }

        private void OnButtonKeyDown(object sender, KeyEventArgs e)
        {
            switch (e.Key)
            {
                case Key.Down:
                    if (_previousDirection != (int)MovingDirection.Upwards)
                        _direction = (int)MovingDirection.Downwards;
                    break;
                case Key.Up:
                    if (_previousDirection != (int)MovingDirection.Downwards)
                        _direction = (int)MovingDirection.Upwards;
                    break;
                case Key.Left:
                    if (_previousDirection != (int)MovingDirection.Toright)
                        _direction = (int)MovingDirection.Toleft;
                    break;
                case Key.Right:
                    if (_previousDirection != (int)MovingDirection.Toleft)
                        _direction = (int)MovingDirection.Toright;
                    break;
            }
            _previousDirection = _direction;
        }

        private void GameOver()
        {
            MessageBox.Show("You Lose! Your score is " + _score, "Game Over", MessageBoxButton.OK, MessageBoxImage.Hand);
            Close();
        }

        // This list describes the Bonus Red pieces of Food on the Canvas
        private readonly List<Point> _bonusPoints = new List<Point>();

        // This list describes the body of the snake on the Canvas
        private readonly List<Point> _snakePoints = new List<Point>();

        private readonly Brush _snakeColor = Brushes.Green;
        private enum Size
        {
            Thin = 4,
            Normal = 6,
            Thick = 8
        };
        private enum MovingDirection
        {
            Upwards = 8,
            Downwards = 2,
            Toleft = 4,
            Toright = 6
        };

        private TimeSpan _fast = new TimeSpan(1);
        private readonly TimeSpan _moderate = new TimeSpan(10000);
        private TimeSpan _slow = new TimeSpan(50000);
        private TimeSpan _damnslow = new TimeSpan(500000);

        private readonly Point _startingPoint = new Point(100, 100);
        private Point _currentPosition;

        private int _direction;

        /* Placeholder for the previous movement direction
         * The snake needs this to avoid its own body.  */
        private int _previousDirection;

        /* Here user can change the size of the snake. 
         * Possible sizes are THIN, NORMAL and THICK */
        private const int HeadSize = (int)Size.Thick;

        private int _score;
        private int _length = 100;
        private readonly Random _random = new Random();
    }
}
