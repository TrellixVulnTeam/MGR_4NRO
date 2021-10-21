using DevExpress.Utils;
using DevExpress.XtraCharts;
using System;
using System.Collections.ObjectModel;
using System.Drawing;
using System.Windows.Forms;

namespace Sprezyna
{
    public partial class Form1 : Form
    {
        const int ViewportPointCount = 100;
        ObservableCollection<DataPoint> positionPoints = new ObservableCollection<DataPoint>();
        ObservableCollection<DataPoint> velocityPoints = new ObservableCollection<DataPoint>();
        ObservableCollection<DataPoint> accelerationPoints = new ObservableCollection<DataPoint>();
        System.Windows.Forms.Timer timer;
        public Form1() { InitializeComponent(); }

        void Form1_Load(object sender, EventArgs e)
        {
            Legend legend1 = new Legend();
            chartControl1.Legends.Add(legend1);

            #region series1
            {
                Color color = Color.Green;

                Series series = new Series();
                series.ChangeView(ViewType.Point);
                series.DataSource = positionPoints;
                series.DataSourceSorted = true;
                series.ArgumentDataMember = "Argument";
                series.ValueDataMembers.AddRange("Value");
                series.ShowInLegend = true;
                chartControl1.Series.Add(series);

                PointSeriesView seriesView = (PointSeriesView)series.View;
                seriesView.LastPoint.LabelDisplayMode = SidePointDisplayMode.DiagramEdge;
                seriesView.LastPoint.Label.TextPattern = "{V:f2}";
                seriesView.AggregateFunction = SeriesAggregateFunction.None;
                seriesView.Color = color;

                CustomLegendItem customLegendItem = new CustomLegendItem();
                customLegendItem.Text = "Position";
                legend1.CustomItems.Add(customLegendItem);
                customLegendItem.MarkerColor = color;

            }
            #endregion
            #region series2
            {
                Color color = Color.Red;

                Series series = new Series();
                series.ChangeView(ViewType.Point);
                series.DataSource = velocityPoints;
                series.DataSourceSorted = true;
                series.ArgumentDataMember = "Argument";
                series.ValueDataMembers.AddRange("Value");
                chartControl1.Series.Add(series);

                PointSeriesView seriesView = (PointSeriesView)series.View;
                seriesView.LastPoint.LabelDisplayMode = SidePointDisplayMode.DiagramEdge;
                seriesView.LastPoint.Label.TextPattern = "{V:f2}";
                seriesView.AggregateFunction = SeriesAggregateFunction.None;
                seriesView.Color = color;

                CustomLegendItem customLegendItem = new CustomLegendItem();
                customLegendItem.Text = "Velocity";
                legend1.CustomItems.Add(customLegendItem);
                customLegendItem.MarkerColor = color;
            }
            #endregion            
            #region series3
            {
                Color color = Color.Blue;

                Series series = new Series();
                series.ChangeView(ViewType.Point);
                series.DataSource = accelerationPoints;
                series.DataSourceSorted = true;
                series.ArgumentDataMember = "Argument";
                series.ValueDataMembers.AddRange("Value");
                chartControl1.Series.Add(series);

                PointSeriesView seriesView = (PointSeriesView)series.View;
                seriesView.LastPoint.LabelDisplayMode = SidePointDisplayMode.DiagramEdge;
                seriesView.LastPoint.Label.TextPattern = "{V:f2}";
                seriesView.AggregateFunction = SeriesAggregateFunction.None;
                seriesView.Color = color;

                CustomLegendItem customLegendItem = new CustomLegendItem();
                customLegendItem.Text = "Acceleration";
                legend1.CustomItems.Add(customLegendItem);
                customLegendItem.MarkerColor = color;
            }
            #endregion

            XYDiagram diagram = (XYDiagram)chartControl1.Diagram;
            diagram.AxisX.DateTimeScaleOptions.ScaleMode = ScaleMode.Continuous;
            diagram.AxisX.Label.ResolveOverlappingOptions.AllowRotate = false;
            diagram.AxisX.Label.ResolveOverlappingOptions.AllowStagger = false;
            diagram.AxisX.WholeRange.SideMarginsValue = 0;
            diagram.DependentAxesYRange = DefaultBoolean.True;
            diagram.AxisY.WholeRange.AlwaysShowZeroLevel = false;

            timer = new System.Windows.Forms.Timer();
            timer.Interval = 30;
            timer.Start();
            timer.Tick += Timer_Tick;
            for (i = -ViewportPointCount; i < 0; ++i)
            {
                positionPoints.Add(new DataPoint(i, 0));
                velocityPoints.Add(new DataPoint(i, 0));
                accelerationPoints.Add(new DataPoint(i, 0));
            }
        }
        void Timer_Tick(object sender, EventArgs e)
        {
            panel1.Invalidate();
            positionPoints.Add(new DataPoint(i, x));
            velocityPoints.Add(new DataPoint(i, v));
            accelerationPoints.Add(new DataPoint(i, a));
            i++;
            if (positionPoints.Count > ViewportPointCount)
            {
                positionPoints.RemoveAt(0);
                velocityPoints.RemoveAt(0);
                accelerationPoints.RemoveAt(0);
            }
        }
        private float x = -1.0f, m = 1.0f;
        private float v = 0.0f, a = 0.0f;
        private int i = 0;

        private void button2_Click(object sender, EventArgs e)
        {
            timer.Stop();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            timer.Start();
        }

        private void chartControl1_Click(object sender, EventArgs e)
        {

        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {
            base.OnPaint(e);
            Panel p = sender as Panel;

            int width = p.Size.Width;
            int height = p.Size.Height;

            int blockSize = width / 2;
            int bias = height / 20;
            int mid = height / 2;

            int amp = mid - (bias + blockSize / 2);

            float k = 0.05f;
            float F = -k * x;
            a = F / m;
            v = v + a;
            x += v;

            int pos = mid + (int)(x * amp) - blockSize / 2;

            int len = pos - bias / 2;


            using (var myPen = new SolidBrush(Color.Aqua))
            {
                var area = new Rectangle(new Point(p.Size.Width / 4, pos), new Size(blockSize, blockSize));
                e.Graphics.FillRectangle(myPen, area);
            }
            using (var brush2 = new SolidBrush(Color.Red))
            {
                var area2 = new Rectangle(new Point(3 * p.Size.Width / 8, bias / 2), new Size(p.Size.Width / 4, len));
                e.Graphics.FillRectangle(brush2, area2);
            }
        }
    }
    public class DataPoint
    {
        public int Argument { get; set; }
        public double Value { get; set; }
        public DataPoint(int argument, double value)
        {
            Argument = argument;
            Value = value;
        }
    }
}
