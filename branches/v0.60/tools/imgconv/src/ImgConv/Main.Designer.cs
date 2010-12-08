namespace ImgConv
{
    partial class Main
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.panel1 = new System.Windows.Forms.Panel();
            this.btnConvert = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.txtSrcHeight = new System.Windows.Forms.TextBox();
            this.txtSrcWidth = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.btnImgSrc = new System.Windows.Forms.Button();
            this.txtFilename = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.txtCustomY = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.txtCustomX = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.cmbImageSize = new System.Windows.Forms.ComboBox();
            this.cmbColorDepth = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.panel2 = new System.Windows.Forms.Panel();
            this.label8 = new System.Windows.Forms.Label();
            this.txtOutY = new System.Windows.Forms.TextBox();
            this.txtOutX = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.picPreview = new System.Windows.Forms.PictureBox();
            this.button1 = new System.Windows.Forms.Button();
            this.btnSaveBinary = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.panel1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.picPreview)).BeginInit();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.btnConvert);
            this.panel1.Controls.Add(this.groupBox2);
            this.panel1.Controls.Add(this.groupBox1);
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(240, 445);
            this.panel1.TabIndex = 2;
            // 
            // btnConvert
            // 
            this.btnConvert.Enabled = false;
            this.btnConvert.Location = new System.Drawing.Point(12, 410);
            this.btnConvert.Name = "btnConvert";
            this.btnConvert.Size = new System.Drawing.Size(202, 23);
            this.btnConvert.TabIndex = 4;
            this.btnConvert.Text = "Convert";
            this.btnConvert.UseVisualStyleBackColor = true;
            this.btnConvert.Click += new System.EventHandler(this.btnConvert_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.txtSrcHeight);
            this.groupBox2.Controls.Add(this.txtSrcWidth);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.btnImgSrc);
            this.groupBox2.Controls.Add(this.txtFilename);
            this.groupBox2.Location = new System.Drawing.Point(4, 3);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(233, 127);
            this.groupBox2.TabIndex = 3;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Image Source:";
            // 
            // txtSrcHeight
            // 
            this.txtSrcHeight.Location = new System.Drawing.Point(78, 72);
            this.txtSrcHeight.Name = "txtSrcHeight";
            this.txtSrcHeight.ReadOnly = true;
            this.txtSrcHeight.Size = new System.Drawing.Size(149, 20);
            this.txtSrcHeight.TabIndex = 7;
            // 
            // txtSrcWidth
            // 
            this.txtSrcWidth.Location = new System.Drawing.Point(78, 46);
            this.txtSrcWidth.Name = "txtSrcWidth";
            this.txtSrcWidth.ReadOnly = true;
            this.txtSrcWidth.Size = new System.Drawing.Size(149, 20);
            this.txtSrcWidth.TabIndex = 6;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(9, 75);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(41, 13);
            this.label6.TabIndex = 4;
            this.label6.Text = "Height:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(9, 49);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(38, 13);
            this.label5.TabIndex = 3;
            this.label5.Text = "Width:";
            // 
            // btnImgSrc
            // 
            this.btnImgSrc.Location = new System.Drawing.Point(78, 98);
            this.btnImgSrc.Name = "btnImgSrc";
            this.btnImgSrc.Size = new System.Drawing.Size(149, 23);
            this.btnImgSrc.TabIndex = 2;
            this.btnImgSrc.Text = "Select Image";
            this.btnImgSrc.UseVisualStyleBackColor = true;
            this.btnImgSrc.Click += new System.EventHandler(this.btnImgSrc_Click);
            // 
            // txtFilename
            // 
            this.txtFilename.Location = new System.Drawing.Point(9, 19);
            this.txtFilename.Name = "txtFilename";
            this.txtFilename.ReadOnly = true;
            this.txtFilename.Size = new System.Drawing.Size(218, 20);
            this.txtFilename.TabIndex = 1;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.txtCustomY);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.txtCustomX);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.cmbImageSize);
            this.groupBox1.Controls.Add(this.cmbColorDepth);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(4, 136);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(233, 106);
            this.groupBox1.TabIndex = 2;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Output Settings:";
            // 
            // txtCustomY
            // 
            this.txtCustomY.Enabled = false;
            this.txtCustomY.Location = new System.Drawing.Point(192, 76);
            this.txtCustomY.Name = "txtCustomY";
            this.txtCustomY.Size = new System.Drawing.Size(35, 20);
            this.txtCustomY.TabIndex = 7;
            this.txtCustomY.Text = "32";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(173, 79);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(17, 13);
            this.label4.TabIndex = 6;
            this.label4.Text = "Y:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(116, 79);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(17, 13);
            this.label3.TabIndex = 5;
            this.label3.Text = "X:";
            // 
            // txtCustomX
            // 
            this.txtCustomX.Enabled = false;
            this.txtCustomX.Location = new System.Drawing.Point(135, 76);
            this.txtCustomX.Name = "txtCustomX";
            this.txtCustomX.Size = new System.Drawing.Size(35, 20);
            this.txtCustomX.TabIndex = 4;
            this.txtCustomX.Text = "32";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 51);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(62, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Image Size:";
            // 
            // cmbImageSize
            // 
            this.cmbImageSize.Enabled = false;
            this.cmbImageSize.FormattingEnabled = true;
            this.cmbImageSize.Items.AddRange(new object[] {
            "240x320 (Keep Ratio)",
            "240x320 (Force Ratio)",
            "Custom"});
            this.cmbImageSize.Location = new System.Drawing.Point(78, 48);
            this.cmbImageSize.Name = "cmbImageSize";
            this.cmbImageSize.Size = new System.Drawing.Size(149, 21);
            this.cmbImageSize.TabIndex = 2;
            this.cmbImageSize.SelectedIndexChanged += new System.EventHandler(this.cmbImageSize_SelectedIndexChanged);
            // 
            // cmbColorDepth
            // 
            this.cmbColorDepth.Enabled = false;
            this.cmbColorDepth.FormattingEnabled = true;
            this.cmbColorDepth.Items.AddRange(new object[] {
            "16-Bit (RGB565)"});
            this.cmbColorDepth.Location = new System.Drawing.Point(78, 20);
            this.cmbColorDepth.Name = "cmbColorDepth";
            this.cmbColorDepth.Size = new System.Drawing.Size(149, 21);
            this.cmbColorDepth.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 23);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(66, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Color Depth:";
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.label8);
            this.panel2.Controls.Add(this.txtOutY);
            this.panel2.Controls.Add(this.txtOutX);
            this.panel2.Controls.Add(this.label7);
            this.panel2.Controls.Add(this.groupBox3);
            this.panel2.Controls.Add(this.button1);
            this.panel2.Controls.Add(this.btnSaveBinary);
            this.panel2.Location = new System.Drawing.Point(246, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(281, 445);
            this.panel2.TabIndex = 3;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(191, 361);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(12, 13);
            this.label8.TabIndex = 7;
            this.label8.Text = "x";
            // 
            // txtOutY
            // 
            this.txtOutY.Enabled = false;
            this.txtOutY.Location = new System.Drawing.Point(207, 359);
            this.txtOutY.Name = "txtOutY";
            this.txtOutY.Size = new System.Drawing.Size(50, 20);
            this.txtOutY.TabIndex = 6;
            // 
            // txtOutX
            // 
            this.txtOutX.Enabled = false;
            this.txtOutX.Location = new System.Drawing.Point(134, 359);
            this.txtOutX.Name = "txtOutX";
            this.txtOutX.Size = new System.Drawing.Size(50, 20);
            this.txtOutX.TabIndex = 5;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(17, 364);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(97, 13);
            this.label7.TabIndex = 4;
            this.label7.Text = "Output Image Size:";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.picPreview);
            this.groupBox3.Location = new System.Drawing.Point(4, 4);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(265, 353);
            this.groupBox3.TabIndex = 3;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Image Preview:";
            // 
            // picPreview
            // 
            this.picPreview.BackColor = System.Drawing.Color.Transparent;
            this.picPreview.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.picPreview.Location = new System.Drawing.Point(13, 21);
            this.picPreview.Name = "picPreview";
            this.picPreview.Size = new System.Drawing.Size(240, 320);
            this.picPreview.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.picPreview.TabIndex = 1;
            this.picPreview.TabStop = false;
            // 
            // button1
            // 
            this.button1.Enabled = false;
            this.button1.Location = new System.Drawing.Point(17, 410);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(240, 23);
            this.button1.TabIndex = 2;
            this.button1.Text = "Export As Header ...";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // btnSaveBinary
            // 
            this.btnSaveBinary.Enabled = false;
            this.btnSaveBinary.Location = new System.Drawing.Point(17, 385);
            this.btnSaveBinary.Name = "btnSaveBinary";
            this.btnSaveBinary.Size = new System.Drawing.Size(240, 23);
            this.btnSaveBinary.TabIndex = 1;
            this.btnSaveBinary.Text = "Export As Binary ...";
            this.btnSaveBinary.UseVisualStyleBackColor = true;
            this.btnSaveBinary.Click += new System.EventHandler(this.btnSaveBinary_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            this.openFileDialog1.FileOk += new System.ComponentModel.CancelEventHandler(this.openFileDialog1_FileOk);
            // 
            // Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(534, 445);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Name = "Main";
            this.Text = "Main";
            this.panel1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.picPreview)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button btnImgSrc;
        private System.Windows.Forms.TextBox txtFilename;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox txtCustomY;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox txtCustomX;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox cmbImageSize;
        private System.Windows.Forms.ComboBox cmbColorDepth;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Button btnSaveBinary;
        private System.Windows.Forms.TextBox txtSrcHeight;
        private System.Windows.Forms.TextBox txtSrcWidth;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.PictureBox picPreview;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Button btnConvert;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox txtOutY;
        private System.Windows.Forms.TextBox txtOutX;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
    }
}