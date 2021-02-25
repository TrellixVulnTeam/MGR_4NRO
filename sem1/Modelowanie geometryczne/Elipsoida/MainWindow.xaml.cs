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
        Timer timer;
        Matrix4 _translateMatrix;
        Matrix4 _scaleMatrix;
        int shaderProgram;
        WriteableBitmap writeableBitmap;

        public MainWindow()
        {
            InitializeComponent();
            timer = new Timer();
            timer.Interval = 10000.0;

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

            Matrix<double> mx = DiagonalMatrix.OfArray(new double[,] {
        {0.0001,0,0,0},
        {0,0.00002,0,0},
        {0,0,0.000005,0},
        {0,0,0,-1}
            });

            try
            {
                // Reserve the back buffer for updates.
                writeableBitmap.Lock();

                unsafe
                {

                    // Compute the pixel's color.
                    int color_data = 255 << 16; // R
                    color_data |= 255 << 8;   // G
                    color_data |= 0 << 0;   // B

                    for (int i = 0; i < writeableBitmap.PixelHeight; ++i)
                    {
                        for (int j = 0; j < writeableBitmap.PixelWidth; ++j)
                        {

                            if (FindZ(mx,j,i).HasValue)
                            {
                                IntPtr pBackBuffer = writeableBitmap.BackBuffer;

                                pBackBuffer += i * writeableBitmap.BackBufferStride;
                                pBackBuffer += j * 4;
                                *((int*)pBackBuffer) = color_data;
                                writeableBitmap.AddDirtyRect(new Int32Rect(j, i, 1, 1));
                            }
                        }

                    }

                }
            }
            finally
            {
                // Release the back buffer and make it available for display.
                writeableBitmap.Unlock();
            }
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
            ErasePixel(e);
        }

        private void i_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            DrawPixel(e);
        }

        private void i_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                DrawPixel(e);
            }
            else if (e.RightButton == MouseButtonState.Pressed)
            {
                ErasePixel(e);
            }
        }

        private void w_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            System.Windows.Media.Matrix m = i.RenderTransform.Value;

            if (e.Delta > 0)
            {
                m.ScaleAt(
                    1.5,
                    1.5,
                    e.GetPosition(i).X,
                    e.GetPosition(i).Y);
            }
            else
            {
                m.ScaleAt(
                    1.0 / 1.5,
                    1.0 / 1.5,
                    e.GetPosition(i).X,
                    e.GetPosition(i).Y);
            }

            i.RenderTransform = new MatrixTransform(m);
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

        private double? FindZ(Matrix<double> matrix, double x, double y)
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

            if (delta < 0) return null;

            if (delta < 1e-9) return -coeff_b / a2;

            var sqrt = Math.Sqrt(delta);

            var s1 = (-coeff_b - sqrt) / a2;
            var s2 = (-coeff_b + sqrt) / a2;

            if (s1 < s2) return s1;
            return s2;
        }

    }
}