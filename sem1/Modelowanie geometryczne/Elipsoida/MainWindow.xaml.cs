using System;
using System.Windows;
using OpenTK.Graphics.OpenGL;
using OpenTK.Graphics;
using System.Timers;
using OpenTK;
using MathNet.Numerics.LinearAlgebra;
using System.Drawing;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Input;
using MathNet.Numerics.LinearAlgebra.Double;
using System.Windows.Threading;
using System.Collections.Generic;
using Point = System.Windows.Point;

namespace Elipsoida
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private bool smthChanged = true;
        DateTime lastFrame;

        DispatcherTimer timer;
        WriteableBitmap writeableBitmap;

        Point moveStartPoint;
        Point rotateStartPoint;
        Matrix<double> M;

        Vector<double> OX = DenseVector.OfArray(new double[] { 1, 0, 0, 0 });
        Vector<double> OY = DenseVector.OfArray(new double[] { 0, 1, 0, 0 });
        Vector<double> OZ = DenseVector.OfArray(new double[] { 0, 0, 1, 0 });

        bool[,] wasYellow;
        Matrix<double> rotationMatrix = DenseMatrix.OfArray(new double[,] {
        {1,0,0,0},
        {0,1,0,0},
        {0,0,1,0},
        { 0,0,0,1}
            });

        Matrix<double> moveMatrix;

        Matrix<double> scaleMatrix = DenseMatrix.OfArray(new double[,] {
        {1,0,0,0},
        {0,1,0,0},
        {0,0,1,0},
        { 0,0,0,1}
            });

        Dictionary<int, int> unitSizes = new Dictionary<int, int>();
        int smallerDimension;

        int selAdaptation;
        int curAdaptation;
        int framesToAdapt = 5;
        int curFrames = 0;

        public MainWindow()
        {
            InitializeComponent();

            RenderOptions.SetBitmapScalingMode(i, BitmapScalingMode.NearestNeighbor);
            RenderOptions.SetEdgeMode(i, EdgeMode.Aliased);


            writeableBitmap = new WriteableBitmap(
                (int)i.Width,
                (int)i.Height,
                96,
                96,
                PixelFormats.Bgr32,
                null);

            i.Source = writeableBitmap;

            i.Stretch = Stretch.None;
            i.HorizontalAlignment = HorizontalAlignment.Left;
            i.VerticalAlignment = VerticalAlignment.Top;

            i.MouseMove += new MouseEventHandler(i_MouseMove);
            i.MouseLeftButtonDown +=
                new MouseButtonEventHandler(i_MouseLeftButtonDown);
            i.MouseRightButtonDown +=
                new MouseButtonEventHandler(i_MouseRightButtonDown);

            window.MouseWheel += new MouseWheelEventHandler(w_MouseWheel);
            lastFrame = DateTime.Now;

            wasYellow = new bool[writeableBitmap.PixelHeight, writeableBitmap.PixelHeight];

            moveMatrix = DenseMatrix.OfArray(new double[,] {
        {1,0,0,-i.Width / 2},
        {0,1,0,-i.Height / 2},
        {0,0,1,0},
        {0,0,0,-1}
            });

            slider_ValueChanged(null, new RoutedPropertyChangedEventArgs<double>(1, 1));

            smallerDimension = (writeableBitmap.PixelHeight < writeableBitmap.PixelWidth ? writeableBitmap.PixelHeight : writeableBitmap.PixelWidth);
            List<int> tmpSizes = new List<int>();
            int parts = 2;
            while (true)
            {
                int size = smallerDimension / parts;
                tmpSizes.Add(size);
                parts *= 2;
                if (size == 1) break;
            }

            adSlider.Maximum = tmpSizes.Count - 1;

            for (int i = 0; i < tmpSizes.Count; ++i)
            {
                unitSizes.Add(i, tmpSizes[tmpSizes.Count - i - 1]);
            }

            timer = new DispatcherTimer();
            timer.Interval = new TimeSpan(0, 0, 0, 0, 1000 / 60);
            timer.Tick += Timer_Tick;
            timer.Start();
        }

        private void Timer_Tick(object sender, EventArgs e)
        {
            Title = (DateTime.Now - lastFrame).TotalMilliseconds.ToString();
            lastFrame = DateTime.Now;

            if (smthChanged) curAdaptation = selAdaptation;
            int unitSize = unitSizes[curAdaptation];

            M = GetTransformations();
            var M_inv = M.Inverse();

            var xRad = xRadiusSlider.Value;
            var yRad = yRadiusSlider.Value;
            var zRad = zRadiusSlider.Value;

            Matrix<double> D = DiagonalMatrix.OfArray(new double[,] {
                {1/(xRad*xRad),0,0,0},
                {0,1/(yRad*yRad),0,0},
                {0,0,1/(zRad*zRad),0},
                {0,0,0,-1}
            });

            var DM = M_inv.Transpose() * D * M_inv;

            try
            {
                writeableBitmap.Lock();

                double obsX = writeableBitmap.PixelWidth / 2;
                double obsY = writeableBitmap.PixelHeight / 2;
                double obsZ = -500;

                for (int i = 0; i + unitSize <= writeableBitmap.PixelHeight; i += unitSize)
                {
                    int unitHeight = i + 2 * unitSize <= writeableBitmap.PixelHeight ? unitSize : writeableBitmap.PixelHeight - i;
                    for (int j = 0; j + unitSize <= writeableBitmap.PixelWidth; j += unitSize)
                    {
                        int unitWidth = j + 2 * unitSize <= writeableBitmap.PixelWidth ? unitSize : writeableBitmap.PixelWidth - j;
                        int x = j + unitWidth / 2;
                        int y = i + unitHeight / 2;
                        var z = FindZ(DM, x, y);
                        if (z.Item1.HasValue)
                        {
                            var toObsV = DenseVector.OfArray(new double[] { obsX - x, obsY - y, obsZ - z.Item1.Value }).Normalize(2);
                            var I = toObsV.DotProduct(z.Item2);
                            
                            if (I < 0)
                            {
                                I = 0;
                            }
                            if (I > 1)
                            {
                                I = 1;
                            }
                            I = Math.Pow(I, mSlider.Value);
                            int intens = (int)(255.0 * I);

                            int
                            color_data = intens << 16; // R
                            color_data |= intens << 8;   // G
                            color_data |= 0 << 0;   // B
                            FillRect(j, i, unitWidth, unitHeight, color_data);
                        }
                        else
                        {
                            FillRect(j, i, unitWidth, unitHeight, 0);
                        }
                    }
                }
            }
            finally
            {
                writeableBitmap.AddDirtyRect(new Int32Rect(0, 0, writeableBitmap.PixelWidth, writeableBitmap.PixelHeight));
                writeableBitmap.Unlock();
                smthChanged = false;
                if (curAdaptation > 0 && curFrames == framesToAdapt)
                {
                    curAdaptation--;
                    curFrames = 0;
                }
                if (curAdaptation > 0) curFrames++;
            }
        }

        private void FillRect(int x, int y, int width, int height, int color)
        {
            unsafe
            {
                IntPtr pBackBuffer;
                for (int i = y; i < y + height; ++i)
                    for (int j = x; j < x + width; ++j)
                    {
                        pBackBuffer = writeableBitmap.BackBuffer;
                        pBackBuffer += i * writeableBitmap.BackBufferStride;
                        pBackBuffer += j * 4;
                        *((int*)pBackBuffer) = color;
                    }
            }
        }

        private Matrix<double> GetTransformations()
        {
            return moveMatrix * rotationMatrix * scaleMatrix;
        }

        private void i_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            rotateStartPoint = e.GetPosition(i);
        }

        private void i_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            moveStartPoint = e.GetPosition(i);
        }

        private void i_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                Move(e.GetPosition(i));
            }
            if (e.RightButton == MouseButtonState.Pressed)
            {
                Rotate(e.GetPosition(i));
            }
        }

        private void w_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            double scale;
            if (e.Delta > 0)
            {
                scale = 1.1 * scaleMatrix[0, 0];
            }
            else
            {
                scale = scaleMatrix[0, 0] / 1.1;
            }

            scaleMatrix[0, 0] = scale;
            scaleMatrix[1, 1] = scale;

            smthChanged = true;
        }

        private (double?, Vector<double>) FindZ(Matrix<double> matrix, double x, double y)
        {
            double a = matrix[0, 0];
            double b = matrix[0, 1];
            double c = matrix[0, 2];
            double d = matrix[0, 3];

            double e = matrix[1, 0];
            double f = matrix[1, 1];
            double g = matrix[1, 2];
            double h = matrix[1, 3];

            double i = matrix[2, 0];
            double j = matrix[2, 1];
            double k = matrix[2, 2];
            double l = matrix[2, 3];

            double m = matrix[3, 0];
            double n = matrix[3, 1];
            double o = matrix[3, 2];
            double p = matrix[3, 3];

            double coeff_a = k;
            double coeff_b = i * x + j * y + c * x + g * y + o + l;
            double coeff_c = a * x * x + e * x * y + m * x + b * x * y + f * y * y + n * y + d * x + h * y + p;

            var delta = coeff_b * coeff_b - 4 * coeff_a * coeff_c;

            var a2 = 2 * coeff_a;

            if (delta < 0) return (null, null);

            double z;

            if (delta < 1e-9) z = -coeff_b / a2;
            else
            {
                var sqrt = Math.Sqrt(delta);

                var s1 = (-coeff_b - sqrt) / a2;
                var s2 = (-coeff_b + sqrt) / a2;

                if (s1 < s2) z = s1;
                else z = s2;
            }

            Vector<double> nabla = DenseVector.OfArray(
                new double[]{
                    2*a*x+e*y+i*z+m+b*y+c*z+d,
                    e*x+b*x+2*f*y+j*z+n+g*z+h,
                    i*x+j*y+c*x+g*y+2*k*z+o+l}
                );

            nabla = nabla.Normalize(2);

            return (z, nabla);

        }

        private void i_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            var curPos = e.GetPosition(i);
            Move(curPos);
        }

        private void Move(Point curPos)
        {
            var xDiff = moveStartPoint.X - curPos.X;
            var yDiff = moveStartPoint.Y - curPos.Y;

            xDiff /= 2;
            yDiff /= 2;

            moveStartPoint = curPos;

            {
                var xABS = Math.Abs(OX[0]);
                var yABS = Math.Abs(OY[0]);
                var zABS = Math.Abs(OZ[0]);

                if (xABS >= yABS && xABS >= zABS)
                {
                    moveMatrix[0, 3] += xDiff;
                }
                else if (yABS >= zABS)
                {
                    moveMatrix[1, 3] += xDiff;
                }
                else
                {
                    moveMatrix[2, 3] += xDiff;
                }
            }

            {
                var xABS = Math.Abs(OX[1]);
                var yABS = Math.Abs(OY[1]);
                var zABS = Math.Abs(OZ[1]);

                if (xABS >= yABS && xABS >= zABS)
                {
                    moveMatrix[0, 3] += yDiff;
                }
                else if (yABS >= zABS)
                {
                    moveMatrix[1, 3] += yDiff;
                }
                else
                {
                    moveMatrix[2, 3] += yDiff;
                }
            }

            smthChanged = true;
        }

        private void i_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            var curPos = e.GetPosition(i);
            Rotate(curPos);
        }

        private void Rotate(Point curPos)
        {

            //v1
            var xDiff = rotateStartPoint.X - curPos.X;
            // var yDiff = rotateStartPoint.Y - curPos.Y;

            //v2
            //var xDiff = curPos.X - rotateStartPoint.X;
            var yDiff = curPos.Y - rotateStartPoint.Y;

            rotateStartPoint = curPos;

            xDiff /= 360;
            yDiff /= 360;

            var sinX = Math.Sin(yDiff);
            var cosX = Math.Cos(yDiff);
            var sinY = Math.Sin(xDiff);
            var cosY = Math.Cos(xDiff);


            var rotX = DenseMatrix.OfArray(new double[,] {
        {1,0,0,0},
        {0,cosX,-sinX,0},
        {0,sinX,cosX,0},
        {0,0,0,1}
            });

            var rotY = DenseMatrix.OfArray(new double[,] {
        {cosY,0,sinY,0},
        {0,1,0,0},
        {-sinY,0,cosY,0},
        {0,0,0,1}
            });
            rotationMatrix = rotY * rotX * rotationMatrix;

            smthChanged = true;
        }

        private void slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            smthChanged = true;

            if (xLabel != null)
                xLabel.Content = "x = " + (int)xRadiusSlider.Value;
            if (yLabel != null)
                yLabel.Content = "y = " + (int)yRadiusSlider.Value;
            if (zLabel != null)
                zLabel.Content = "z = " + (int)zRadiusSlider.Value;
            if (mLabel != null)
                mLabel.Content = "m = " + (int)mSlider.Value;
            if (adLabel != null)
            {
                adLabel.Content = "adaptive level = " + (int)adSlider.Value;
                selAdaptation = (int)adSlider.Value;
            }
        }
    }
}