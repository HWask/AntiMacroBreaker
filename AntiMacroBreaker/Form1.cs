using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using AntiMacroCLR;

namespace AntiMacroBreaker
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        bool StopThread = false;
        bool GameWindowFound = false;

        private void Form1_Load(object sender, EventArgs e)
        {
            Icon = Icon.FromHandle(Properties.Resources.icon.Handle);
            BackgroundImage = Properties.Resources.ahorn;

            notifyIcon1.Visible = true;
            notifyIcon1.Icon = Icon.FromHandle(Properties.Resources.icon.Handle);
        }

        private void notifyIcon1_MouseClick(object sender, MouseEventArgs e)
        {
            if (!Visible)
            {
                Show();
                WindowState = FormWindowState.Normal;
            }
        }

        private void Form1_Resize(object sender, EventArgs e)
        {
            if (WindowState == FormWindowState.Minimized)
            {
                Hide();

                notifyIcon1.BalloonTipTitle = "Macro Breaker";
                notifyIcon1.BalloonTipText = "Macro Breaker is running in the background";
                notifyIcon1.ShowBalloonTip(1000);
            }
        }

        private void Form1_Shown(object sender, EventArgs e)
        {
            new System.Threading.Thread(() => ThreadFunc()).Start();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            StopThread = true;
        }

        private void ThreadFunc()
        {
            while(!StopThread)
            {
                if(MacroBreaker.IsWindowAlive("Ashen Empires"))
                {
                    if (!GameWindowFound)
                    {
                        NotifyGui("Game has been started");
                        GameWindowFound = true;
                    }

                }
                else
                {
                    if(GameWindowFound)
                    {
                        NotifyGui("Game has been closed");
                        GameWindowFound = false;
                    }
                }

                if(GameWindowFound)
                {
                    Bitmap image = MacroBreaker.CaptureImage("Ashen Empires");
                    if(MacroBreaker.IsMacroPopped(image))
                    {
                        var points = MacroBreaker.Solve(image);
                        foreach(var p in points)
                        {
                            MacroBreaker.SimulateMouseClick("Ashen Empires", p);
                        }

                        NotifyGui("Anti-Macro has been solved");
                    }
                }

                System.Threading.Thread.Sleep(300);
            }
        }

        private void NotifyGui(String text)
        {
            BeginInvoke(new Action(() =>
            {
                notifyIcon1.BalloonTipTitle = "Macro Breaker";
                notifyIcon1.BalloonTipText = text;
                notifyIcon1.ShowBalloonTip(1000);
            }));
        }
    }
}
