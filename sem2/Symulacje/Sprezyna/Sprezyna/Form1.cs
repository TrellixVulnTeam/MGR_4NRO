using DevExpress.Utils;
using DevExpress.XtraCharts;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Sprezyna
{
    public partial class Form1 : Form
    {
        int ViewportPointCount = 300;
        ObservableQueue<DataPoint> positionPoints = new ObservableQueue<DataPoint>();
        ObservableQueue<DataPoint> velocityPoints = new ObservableQueue<DataPoint>();
        ObservableQueue<DataPoint> accelerationPoints = new ObservableQueue<DataPoint>();
        ObservableQueue<DataPoint> fPoints = new ObservableQueue<DataPoint>();
        ObservableQueue<DataPoint> gPoints = new ObservableQueue<DataPoint>();
        ObservableQueue<DataPoint> hPoints = new ObservableQueue<DataPoint>();
        ObservableQueue<DataPoint> wPoints = new ObservableQueue<DataPoint>();
        ObservableCollection<DataPoint> statePoints = new ObservableCollection<DataPoint>();
        Timer timer;
        private double time = 0.0f;
        private double x = 0.0f, m = 1.0f;
        private double v = 0.0f, a = 0.0f, v0 = 0, x0 = 1;
        private double f, g;
        private double c = 1, k = 0;
        private int delta = 30;
        private int i = 0;
        private double xPrev, xPrev2, vPrev, diff;
        private bool firstTick = true;
        private double minArg = -10, maxArg = 0;
        private double w = 0, h = 0;

        private double Const(double t, double v)
        {
            return v;
        }
        private double Step(double t, double A, double t0)
        {
            return t >= t0 ? A : 0;
        }
        private double Step2(double t, double A, double omega, double phi)
        {
            return A * Math.Sign(Math.Sin(omega * t + phi));
        }
        private double Sinus(double t, double A, double omega, double phi)
        {
            return A * Math.Sin(omega * t + phi);
        }
        private double H(double t)
        {
            return Const(t,0);
            return Step(t,1,0);
            return Step2(t,1,1,0);
            return Sinus(t,1,1,0);
        }
        private double W(double t)
        {
            return Const(t, 0);
            return Step(t, 1, 0);
            return Step2(t, 1, 1, 0);
            return Sinus(t, 1, 1, 0);
        }
        public Form1() { InitializeComponent(); }

        void Form1_Load(object sender, EventArgs e)
        {
            #region diagram1
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
                seriesView.PointMarkerOptions.Size = 2;

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
                seriesView.PointMarkerOptions.Size = 2;

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
                seriesView.PointMarkerOptions.Size = 2;

                CustomLegendItem customLegendItem = new CustomLegendItem();
                customLegendItem.Text = "Acceleration";
                legend1.CustomItems.Add(customLegendItem);
                customLegendItem.MarkerColor = color;
            }
            #endregion

            XYDiagram diagram = (XYDiagram)chartControl1.Diagram;
            diagram.AxisX.WholeRange.SetMinMaxValues(minArg, maxArg);
            diagram.AxisX.Label.ResolveOverlappingOptions.AllowRotate = false;
            diagram.AxisX.Label.ResolveOverlappingOptions.AllowStagger = false;
            diagram.DependentAxesYRange = DefaultBoolean.True;
            diagram.AxisY.WholeRange.AlwaysShowZeroLevel = true;
            diagram.AxisY.WholeRange.Auto = false;
            diagram.AxisY.WholeRange.MinValue = -1;
            diagram.AxisY.WholeRange.MaxValue = 1;

            #endregion
            #region diagram2
            Legend legend2 = new Legend();
            chartControl2.Legends.Add(legend2);

            #region series1
            {
                Color color = Color.Green;

                Series series = new Series();
                series.ChangeView(ViewType.Point);
                series.DataSource = fPoints;
                series.DataSourceSorted = true;
                series.ArgumentDataMember = "Argument";
                series.ValueDataMembers.AddRange("Value");
                series.ShowInLegend = true;
                chartControl2.Series.Add(series);

                PointSeriesView seriesView = (PointSeriesView)series.View;
                seriesView.LastPoint.LabelDisplayMode = SidePointDisplayMode.DiagramEdge;
                seriesView.LastPoint.Label.TextPattern = "{V:f2}";
                seriesView.AggregateFunction = SeriesAggregateFunction.None;
                seriesView.Color = color;
                seriesView.PointMarkerOptions.Size = 2;

                CustomLegendItem customLegendItem = new CustomLegendItem();
                customLegendItem.Text = "F";
                legend2.CustomItems.Add(customLegendItem);
                customLegendItem.MarkerColor = color;

            }
            #endregion
            #region series2
            {
                Color color = Color.Red;

                Series series = new Series();
                series.ChangeView(ViewType.Point);
                series.DataSource = gPoints;
                series.DataSourceSorted = true;
                series.ArgumentDataMember = "Argument";
                series.ValueDataMembers.AddRange("Value");
                chartControl2.Series.Add(series);

                PointSeriesView seriesView = (PointSeriesView)series.View;
                seriesView.LastPoint.LabelDisplayMode = SidePointDisplayMode.DiagramEdge;
                seriesView.LastPoint.Label.TextPattern = "{V:f2}";
                seriesView.AggregateFunction = SeriesAggregateFunction.None;
                seriesView.Color = color;
                seriesView.PointMarkerOptions.Size = 2;

                CustomLegendItem customLegendItem = new CustomLegendItem();
                customLegendItem.Text = "G";
                legend2.CustomItems.Add(customLegendItem);
                customLegendItem.MarkerColor = color;
            }
            #endregion
            #region series3
            {
                Color color = Color.Blue;

                Series series = new Series();
                series.ChangeView(ViewType.Point);
                series.DataSource = hPoints;
                series.DataSourceSorted = true;
                series.ArgumentDataMember = "Argument";
                series.ValueDataMembers.AddRange("Value");
                chartControl2.Series.Add(series);

                PointSeriesView seriesView = (PointSeriesView)series.View;
                seriesView.LastPoint.LabelDisplayMode = SidePointDisplayMode.DiagramEdge;
                seriesView.LastPoint.Label.TextPattern = "{V:f2}";
                seriesView.AggregateFunction = SeriesAggregateFunction.None;
                seriesView.Color = color;
                seriesView.PointMarkerOptions.Size = 2;

                CustomLegendItem customLegendItem = new CustomLegendItem();
                customLegendItem.Text = "H";
                legend2.CustomItems.Add(customLegendItem);
                customLegendItem.MarkerColor = color;
            }
            #endregion  

            diagram = (XYDiagram)chartControl2.Diagram;
            diagram.AxisX.WholeRange.SetMinMaxValues(minArg, maxArg);
            diagram.AxisX.Label.ResolveOverlappingOptions.AllowRotate = false;
            diagram.AxisX.Label.ResolveOverlappingOptions.AllowStagger = false;
            diagram.DependentAxesYRange = DefaultBoolean.True;
            diagram.AxisY.WholeRange.AlwaysShowZeroLevel = true;
            diagram.AxisY.WholeRange.Auto = false;
            diagram.AxisY.WholeRange.MinValue = -1;
            diagram.AxisY.WholeRange.MaxValue = 1;

            #endregion
            #region diagram3
            Legend legend3 = new Legend();
            chartControl3.Legends.Add(legend3);

            #region series1
            {
                Color color = Color.Red;

                Series series = new Series();
                series.ChangeView(ViewType.Point);
                series.DataSource = wPoints;
                series.DataSourceSorted = true;
                series.ArgumentDataMember = "Argument";
                series.ValueDataMembers.AddRange("Value");
                series.ShowInLegend = true;
                chartControl3.Series.Add(series);

                PointSeriesView seriesView = (PointSeriesView)series.View;
                seriesView.LastPoint.LabelDisplayMode = SidePointDisplayMode.DiagramEdge;
                seriesView.LastPoint.Label.TextPattern = "{V:f2}";
                seriesView.AggregateFunction = SeriesAggregateFunction.None;
                seriesView.Color = color;
                seriesView.PointMarkerOptions.Size = 2;

                CustomLegendItem customLegendItem = new CustomLegendItem();
                customLegendItem.Text = "W";
                legend3.CustomItems.Add(customLegendItem);
                customLegendItem.MarkerColor = color;

            }
            #endregion

            diagram = (XYDiagram)chartControl3.Diagram;
            diagram.AxisX.WholeRange.SetMinMaxValues(minArg, maxArg);
            diagram.AxisX.Label.ResolveOverlappingOptions.AllowRotate = false;
            diagram.AxisX.Label.ResolveOverlappingOptions.AllowStagger = false;
            diagram.DependentAxesYRange = DefaultBoolean.True;
            diagram.AxisY.WholeRange.AlwaysShowZeroLevel = true;
            diagram.AxisY.WholeRange.Auto = false;
            diagram.AxisY.WholeRange.MinValue = -1;
            diagram.AxisY.WholeRange.MaxValue = 1;
            #endregion
            #region diagram4
            #region series1
            {
                Color color = Color.Green;

                Series series = new Series();
                series.ChangeView(ViewType.Point);
                series.DataSource = statePoints;
                series.DataSourceSorted = true;
                series.ArgumentDataMember = "Argument";
                series.ValueDataMembers.AddRange("Value");
                series.ShowInLegend = true;
                chartControl4.Series.Add(series);

                PointSeriesView seriesView = (PointSeriesView)series.View;
                seriesView.LastPoint.LabelDisplayMode = SidePointDisplayMode.DiagramEdge;
                seriesView.LastPoint.Label.TextPattern = "{V:f2}";
                seriesView.AggregateFunction = SeriesAggregateFunction.None;
                seriesView.Color = color;
                seriesView.PointMarkerOptions.Size = 2;
            }
            #endregion

            diagram = (XYDiagram)chartControl4.Diagram;
            diagram.AxisX.DateTimeScaleOptions.AggregateFunction = AggregateFunction.None;
            diagram.AxisX.DateTimeScaleOptions.MeasureUnit = DateTimeMeasureUnit.Millisecond;
            diagram.AxisX.WholeRange.SetMinMaxValues(-1, 1);
            diagram.AxisX.Label.ResolveOverlappingOptions.AllowRotate = false;
            diagram.AxisX.Label.ResolveOverlappingOptions.AllowStagger = false;
            diagram.DependentAxesYRange = DefaultBoolean.True;
            diagram.AxisY.WholeRange.AlwaysShowZeroLevel = true;
            diagram.AxisY.WholeRange.Auto = false;
            diagram.AxisY.WholeRange.MinValue = -1;
            diagram.AxisY.WholeRange.MaxValue = 1;
            #endregion

            timer = new Timer();
            timer.Tick += Timer_Tick;
        }
        void Timer_Tick(object sender, EventArgs e)
        {
            time += diff;
            panel1.Invalidate();
            h = H(time);
            w = W(time);
            if (firstTick)
            {
                x = x0;
                v = v0;

                xPrev2 = x;

                a = (c * (w - x) - k * v + h) / m;
                x = x + v0 * diff;
                f = c * (w - x);
                g = -k * v;

                xPrev = x;
                vPrev = v;
                firstTick = false;
            }
            else
            {

            https://www.wolframalpha.com/input/?i=solve+%28x-2*j%2Bi%29%2F%28d*d%29%3D%28c*%28w-j%29-k*%28%28x-i%29%2F%282*d%29%29%2Bh%29%2Fm+for+x&assumption=%22i%22+-%3E+%22Variable%22
                double diff2 = diff * diff;
                double div = diff * k + 2 * m;
                double temp = 2 * diff2 * (c * (w - xPrev) + h) +
                            xPrev2 * diff * k -
                            2 * m * (xPrev2 - 2 * xPrev);

                xPrev2 = x;
                x = temp / div;
                v = (x - xPrev) / diff;
                a = (v - vPrev) / diff;
                f = c * (w - x);
                g = -k * v;
                xPrev = x;
                vPrev = v;
            }

            positionPoints.Enqueue(new DataPoint(time, x));
            velocityPoints.Enqueue(new DataPoint(time, v));
            accelerationPoints.Enqueue(new DataPoint(time, a));
            fPoints.Enqueue(new DataPoint(time, f));
            gPoints.Enqueue(new DataPoint(time, g));
            hPoints.Enqueue(new DataPoint(time, h));
            wPoints.Enqueue(new DataPoint(time, w));
            statePoints.Add(new DataPoint(x, v));
            i++;
            if (i > ViewportPointCount)
            {
                positionPoints.Dequeue();
                velocityPoints.Dequeue();
                accelerationPoints.Dequeue();
                fPoints.Dequeue();
                gPoints.Dequeue();
                hPoints.Dequeue();
                wPoints.Dequeue();
                i--;
            }
            XYDiagram diagram = (XYDiagram)chartControl1.Diagram;
            diagram.AxisX.WholeRange.SetMinMaxValues(time - 10, time);
            diagram = (XYDiagram)chartControl2.Diagram;
            diagram.AxisX.WholeRange.SetMinMaxValues(time - 10, time);
            diagram = (XYDiagram)chartControl3.Diagram;
            diagram.AxisX.WholeRange.SetMinMaxValues(time - 10, time);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            timer.Stop();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            delta = (int)delta_ctrl.Value;
            x0 = (double)x0_ctrl.Value;
            v0 = (double)v0_ctrl.Value;
            c = (double)c_ctrl.Value;
            k = (double)k_ctrl.Value;
            m = (double)m_ctrl.Value;
            timer.Interval = delta;
            diff = (double)delta / 1000;
            ViewportPointCount = 10000 / delta;
            timer.Start();
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

            int pos = mid + (int)(x * amp) - blockSize / 2;
            int pos2 = mid + (int)((w-1.0f) * amp) - blockSize / 2;

            int len = pos - pos2;
            if (len < 0) len = 0;

            using (var myPen = new SolidBrush(Color.Aqua))
            {
                var area = new Rectangle(new Point(p.Size.Width / 4, pos), new Size(blockSize, blockSize));
                e.Graphics.FillRectangle(myPen, area);
            }
            using (var brush2 = new SolidBrush(Color.Red))
            {
                var area2 = new Rectangle(new Point(3 * p.Size.Width / 8, pos2), new Size(p.Size.Width / 4, len));
                e.Graphics.FillRectangle(brush2, area2);
            }
        }
    }
    public class DataPoint
    {
        public double Argument { get; set; }
        public double Value { get; set; }
        public DataPoint(double argument, double value)
        {
            Argument = argument;
            Value = value;
        }
    }

    public class ObservableQueue<T> : Queue<T>, INotifyCollectionChanged, INotifyPropertyChanged
    {
        public ObservableQueue()
        {
        }

        public ObservableQueue(IEnumerable<T> collection)
        {
            foreach (var item in collection)
                base.Enqueue(item);
        }

        public ObservableQueue(List<T> list)
        {
            foreach (var item in list)
                base.Enqueue(item);
        }


        public new virtual void Clear()
        {
            base.Clear();
            this.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
        }

        public new virtual T Dequeue()
        {
            var item = base.Dequeue();
            this.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, item));
            return item;
        }

        public new virtual void Enqueue(T item)
        {
            base.Enqueue(item);
            this.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, item));
        }


        public virtual event NotifyCollectionChangedEventHandler CollectionChanged;


        protected virtual void OnCollectionChanged(NotifyCollectionChangedEventArgs e)
        {
            this.RaiseCollectionChanged(e);
        }

        protected virtual void OnPropertyChanged(PropertyChangedEventArgs e)
        {
            this.RaisePropertyChanged(e);
        }


        protected virtual event PropertyChangedEventHandler PropertyChanged;


        private void RaiseCollectionChanged(NotifyCollectionChangedEventArgs e)
        {
            if (this.CollectionChanged != null)
                this.CollectionChanged(this, e);
        }

        private void RaisePropertyChanged(PropertyChangedEventArgs e)
        {
            if (this.PropertyChanged != null)
                this.PropertyChanged(this, e);
        }


        event PropertyChangedEventHandler INotifyPropertyChanged.PropertyChanged
        {
            add { this.PropertyChanged += value; }
            remove { this.PropertyChanged -= value; }
        }
    }
}
