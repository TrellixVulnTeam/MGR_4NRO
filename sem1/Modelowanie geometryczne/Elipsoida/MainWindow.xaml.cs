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

namespace Elipsoida
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private int _numOfVertices = 0;
        private int _gameFieldWidth = 20;
        private int _gameFieldHeight = 20;
        private int _uColorLocation;
        private int _uTranslateMatrixLocation;
        private int _uScaleMatrixLocation;

        float[] vertices;
        DispatcherTimer timer;
        Matrix4 _translateMatrix;
        Matrix4 _scaleMatrix;
        int shaderProgram;
        WriteableBitmap writeableBitmap;

        double scale = 1;
        System.Windows.Point moveStartPoint;
        System.Windows.Point rotateStartPoint;
        Matrix<double> M;

        Vector<double> OX = DenseVector.OfArray(new double[] { 1, 0, 0, 0 });
        Vector<double> OY = DenseVector.OfArray(new double[] { 0, 1, 0, 0 });
        Vector<double> OZ = DenseVector.OfArray(new double[] { 0, 0, 1, 0 });

        double moveX = -400;
        double moveY = -400;
        double moveZ = 0;

        double rotateX = 0;
        double rotateY = 0;
        double rotateZ = 0;

        public MainWindow()
        {
            InitializeComponent();
            timer = new DispatcherTimer();
            timer.Interval = new TimeSpan(0, 0, 0, 0, 1000 / 60);
            timer.Tick += Timer_Tick;
            timer.Start();

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

        }

        private void Timer_Tick(object sender, EventArgs e)
        {
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

                unsafe
                {

                    byte[] black = { 0, 0, 0, 0 }; // B G R


                  //  color_data &= 11 << 24;   // G

                    double obsX = writeableBitmap.PixelWidth / 2;
                    double obsY = writeableBitmap.PixelHeight / 2;
                    double obsZ = 1240;

                    for (int i = 0; i < writeableBitmap.PixelHeight; ++i)
                    {
                        for (int j = 0; j < writeableBitmap.PixelWidth; ++j)
                        {
                            var z = FindZ(DM, j, i);
                            if (z.Item1.HasValue)
                            {
                                var toObsV = DenseVector.OfArray(new double[] { j - obsX, i - obsY, z.Item1.Value - obsZ}).Normalize(2);

                                var I = toObsV.DotProduct(z.Item2);
                                if(I>1 || I<0)
                                {
                                    var a = 1;
                                }
                                int intens = (int)(255.0 * I);

                                int
                                color_data = intens << 16; // R
                                color_data |= intens << 8;   // G
                                color_data |= 0 << 0;   // B

                                IntPtr pBackBuffer = writeableBitmap.BackBuffer;

                                pBackBuffer += i * writeableBitmap.BackBufferStride;
                                pBackBuffer += j * 4;
                                *((int*)pBackBuffer) = color_data;
                                writeableBitmap.AddDirtyRect(new Int32Rect(j, i, 1, 1));
                            }
                            else
                            {
                                Int32Rect rect = new Int32Rect(j, i, 1, 1);
                                writeableBitmap.WritePixels(rect, black, 4, 0);
                            }
                        }
                    }
                }
            }
            finally
            {
                writeableBitmap.Unlock();
            }
        }

        private Matrix<double> GetTransformations()
        {
            var sinX = Math.Sin(rotateX);
            var cosX = Math.Cos(rotateX);
            var sinY = Math.Sin(rotateY);
            var cosY = Math.Cos(rotateY);
            var sinZ = Math.Sin(rotateZ);
            var cosZ = Math.Cos(rotateZ);


            var rotX = DenseMatrix.OfArray(new double[,] {
        {1,0,0,0},
        {0,cosX,-sinX,0},
        {0,sinX,cosX,0},
        {0,0,0,1}
            });

            var rotY = DenseMatrix.OfArray(new double[,] {
        {cosX,0,sinX,0},
        {0,1,0,0},
        {-sinX,0,cosX,0},
        {0,0,0,1}
            });

            var rotZ = DenseMatrix.OfArray(new double[,] {
        {cosX,-sinX,0,0},
        {sinX,cosX,0,0},
        {0,0,1,0},
        {0,0,0,1}
            });
            var scaleM = DiagonalMatrix.OfDiagonal(4, 4, new List<double>() { scale, scale, scale, 1 });

            var move = DenseMatrix.OfArray(new double[,] {
        {1,0,0,moveX},
        {0,1,0,moveY},
        {0,0,1,moveZ},
        {0,0,0,-1}
            });

            return move * scaleM * rotZ * rotY * rotX;
        }

        private void DrawPixel(MouseEventArgs e)
        {
            int column = (int)e.GetPosition(i).X;
            int row = (int)e.GetPosition(i).Y;

            try
            {
                // Reserve the back buffer for updates.
                writeableBitmap.Lock();

                unsafe
                {
                    // Get a pointer to the back buffer.
                    IntPtr pBackBuffer = writeableBitmap.BackBuffer;

                    // Find the address of the pixel to draw.
                    pBackBuffer += row * writeableBitmap.BackBufferStride;
                    pBackBuffer += column * 4;

                    // Compute the pixel's color.
                    int color_data = 255 << 16; // R
                    color_data |= 128 << 8;   // G
                    color_data |= 255 << 0;   // B

                    // Assign the color data to the pixel.
                    *((int*)pBackBuffer) = color_data;
                }

                // Specify the area of the bitmap that changed.
                writeableBitmap.AddDirtyRect(new Int32Rect(column, row, 1, 1));
            }
            finally
            {
                // Release the back buffer and make it available for display.
                writeableBitmap.Unlock();
            }
        }

        private void ErasePixel(MouseEventArgs e)
        {
            byte[] ColorData = { 0, 0, 0, 0 }; // B G R

            Int32Rect rect = new Int32Rect(
                    (int)(e.GetPosition(i).X),
                    (int)(e.GetPosition(i).Y),
                    1,
                    1);

            writeableBitmap.WritePixels(rect, ColorData, 4, 0);
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

        }

        private void w_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            if (e.Delta > 0)
            {
                scale *= 1.1;
            }
            else
            {
                scale /= 1.1;
            }
        }
        /*
        private void Timer_Elapsed(object sender, ElapsedEventArgs e)
        {
            glControl.Invalidate();
        }

        private void glControl_Load(object sender, EventArgs e)
        {
            glControl.MakeCurrent();

            // Get a shader program ID
            shaderProgram = InitShadersAndGetProgram();

            _numOfVertices = InitVertexBuffers();

            _uColorLocation = GL.GetUniformLocation(shaderProgram, "uColor");
            if (_uColorLocation < 0)
            {
                MessageBox.Show("Failed to get uColorLocation variable");
                return;
            }

            // Set a coordinate cell
            int uProjMatrixLocation = GL.GetUniformLocation(shaderProgram, "uProjMatrix");
            if (uProjMatrixLocation < 0)
            {
                MessageBox.Show("Failed to get a location uProjMatrix variable");
                return;
            }
            Matrix4 projMatrix = Matrix4.CreateOrthographicOffCenter(0f, _gameFieldWidth, _gameFieldHeight, 0f, -100f, 100f);
            GL.UniformMatrix4(uProjMatrixLocation, false, ref projMatrix);

            // Get uScaleMatrix Location
            _uScaleMatrixLocation = GL.GetUniformLocation(shaderProgram, "uScaleMatrix");
            if (_uScaleMatrixLocation < 0)
            {
                MessageBox.Show("Failed to get a location uScaleMatrix variable");
                return;
            }
            _scaleMatrix = new Matrix4();

            // Get uTranslateMatrix Location
            _uTranslateMatrixLocation = GL.GetUniformLocation(shaderProgram, "uTranslateMatrix");
            if (_uTranslateMatrixLocation < 0)
            {
                MessageBox.Show("Failed to get a location uTranslateMatrix variable");
                return;
            }
            _translateMatrix = new Matrix4();


            // Set a triangle color
            GL.Uniform3(_uColorLocation, 0.945f, 0.745f, 0.356f);

            // Set a color for clearing the glCotrol
            GL.ClearColor(new Color4(0.286f, 0.576f, 0.243f, 1f));
        }

        private void glControl_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
        {
            GL.Viewport(0, 0, glControl.Width, glControl.Height);

            // Clear the glControl with set color
            GL.Clear(ClearBufferMask.ColorBufferBit);

            Random r = new Random();

            if (_numOfVertices != 0)
            {
                _scaleMatrix = Matrix4.CreateScale(5);
                GL.UniformMatrix4(_uScaleMatrixLocation, false, ref _scaleMatrix);
                _translateMatrix = Matrix4.CreateTranslation(new Vector3((float)r.NextDouble() + 10, (float)r.NextDouble() + 10, 1f));
                GL.UniformMatrix4(_uTranslateMatrixLocation, false, ref _translateMatrix);
                GL.DrawArrays(PrimitiveType.Triangles, 0, _numOfVertices);
            }

            // Swap the front and back buffers
            glControl.SwapBuffers();
        }

        private int InitVertexBuffers()
        {
            vertices = new float[]
            {
                0.0f, 0.5f,
                -0.5f, -0.5f,
                0.5f, -0.5f
            };
            int n = 3;

            int vbo;
            GL.GenBuffers(1, out vbo);

            // Get an array size in bytes
            GL.BindBuffer(BufferTarget.ArrayBuffer, vbo);
            int sizeInBytes = vertices.Length * sizeof(float);
            // Send the vertex array to a video card memory
            GL.BufferData(BufferTarget.ArrayBuffer, sizeInBytes, vertices, BufferUsageHint.StaticDraw);
            // Config the aPosition variable
            GL.VertexAttribPointer(0, 2, VertexAttribPointerType.Float, false, 0, 0);
            GL.EnableVertexAttribArray(0);


            return n;
        }

        private int InitShadersAndGetProgram()
        {
            string vertexShaderSource =
                "#version 140\n" +
                "in vec2 aPosition;" +
                "uniform mat4 uProjMatrix;" +
                "uniform mat4 uScaleMatrix;" +
                "uniform mat4 uTranslateMatrix;" +
                "void main()" +
                "{" +
                "    gl_Position = uProjMatrix * uTranslateMatrix * uScaleMatrix * vec4(aPosition, 1.0, 1.0);" +
                "}";

            string fragmentShaderSource =
                "#version 140\n" +
                "out vec4 fragColor;" +
                "uniform vec3 uColor;" +
                "void main()" +
                "{" +
                "    fragColor = vec4(uColor, 1.0);" +
                "}";

            // Vertex Shader
            int vShader = GL.CreateShader(ShaderType.VertexShader);
            GL.ShaderSource(vShader, vertexShaderSource);
            GL.CompileShader(vShader);
            // Check compilation
            int ok;
            GL.GetShader(vShader, ShaderParameter.CompileStatus, out ok);
            if (ok == 0)
            {
                string vShaderInfo = GL.GetShaderInfoLog(vShader);
                MessageBox.Show("Error in the vertex shader:\n" + vShaderInfo);
                return -1;
            }

            // Fragment Shader
            int fShader = GL.CreateShader(ShaderType.FragmentShader);
            GL.ShaderSource(fShader, fragmentShaderSource);
            GL.CompileShader(fShader);
            GL.GetShader(fShader, ShaderParameter.CompileStatus, out ok);
            if (ok == 0)
            {
                string fShaderInfo = GL.GetShaderInfoLog(fShader);
                MessageBox.Show("Error in the fragment shader:\n" + fShaderInfo);
                return -1;
            }

            int program = GL.CreateProgram();
            GL.AttachShader(program, vShader);
            GL.AttachShader(program, fShader);
            GL.LinkProgram(program);
            GL.UseProgram(program);

            return program;
        }

        private void buttonSetBGColor_Click(object sender, RoutedEventArgs e)
        {
            var dialog = new System.Windows.Forms.ColorDialog();

            if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                GL.ClearColor(dialog.Color);
                glControl.Invalidate();
            }

        }

        private void buttonSetTRColor_Click(object sender, RoutedEventArgs e)
        {
            var dialog = new System.Windows.Forms.ColorDialog();

            if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                float r = dialog.Color.R / 255f;
                float g = dialog.Color.G / 255f;
                float b = dialog.Color.B / 255f;
                GL.Uniform3(_uColorLocation, r, g, b);
                glControl.Invalidate();
            }

        }

        private void glControl_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
        {
            if (e.KeyCode == System.Windows.Forms.Keys.W)
            {
                vertices[1] += 0.1f;
            }
            if (e.KeyCode == System.Windows.Forms.Keys.W)
            {
                vertices[1] -= 0.1f;
            }
        }
        */

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
            var xDiff = moveStartPoint.X - curPos.X;
            var yDiff = moveStartPoint.Y - curPos.Y;

            {
                var xABS = Math.Abs(OX[0]);
                var yABS = Math.Abs(OY[0]);
                var zABS = Math.Abs(OZ[0]);

                if (xABS >= yABS && xABS >= zABS)
                {
                    moveX += xDiff / 10;
                }
                else if (yABS >= zABS)
                {
                    moveY += xDiff / 10;
                }
                else
                {
                    moveZ += xDiff / 10;
                }
            }

            {
                var xABS = Math.Abs(OX[1]);
                var yABS = Math.Abs(OY[1]);
                var zABS = Math.Abs(OZ[1]);

                if (xABS >= yABS && xABS >= zABS)
                {
                    moveX += yDiff / 10;
                }
                else if (yABS >= zABS)
                {
                    moveY += yDiff / 10;
                }
                else
                {
                    moveZ += yDiff / 10;
                }
            }
        }

        private void i_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            var curPos = e.GetPosition(i);
            var xDiff = moveStartPoint.X - curPos.X;
            var yDiff = moveStartPoint.Y - curPos.Y;

            {
                var xABS = Math.Abs(OX[1]);
                var yABS = Math.Abs(OY[1]);
                var zABS = Math.Abs(OZ[1]);

                if (xABS >= yABS && xABS >= zABS)
                {
                    rotateX += xDiff / 10;
                }
                else if (yABS >= zABS)
                {
                    rotateY += xDiff / 10;
                }
                else
                {
                    rotateZ += xDiff / 10;
                }
            }

            {
                var xABS = Math.Abs(OX[0]);
                var yABS = Math.Abs(OY[0]);
                var zABS = Math.Abs(OZ[0]);

                if (xABS >= yABS && xABS >= zABS)
                {
                    rotateX += yDiff / 10;
                }
                else if (yABS >= zABS)
                {
                    rotateY += yDiff / 10;
                }
                else
                {
                    rotateZ += yDiff / 10;
                }
            }
        }
    }
}