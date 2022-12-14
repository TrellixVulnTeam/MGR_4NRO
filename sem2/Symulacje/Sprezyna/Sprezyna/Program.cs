using DevExpress.XtraEditors;
using System;
using System.Windows.Forms;

namespace Sprezyna {
    static class Program {

        [STAThread]
        static void Main() {
            WindowsFormsSettings.SetDPIAware();
            WindowsFormsSettings.AllowDpiScale = true;
            WindowsFormsSettings.AllowAutoScale = DevExpress.Utils.DefaultBoolean.True;
            WindowsFormsSettings.ForceDirectXPaint();
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());

        }
    }
}
