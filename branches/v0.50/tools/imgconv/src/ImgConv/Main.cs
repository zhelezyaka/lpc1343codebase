using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace ImgConv
{
    public partial class Main : Form
    {
        public Main()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Handles the Click event of the btnSaveBinary control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void btnSaveBinary_Click(object sender, EventArgs e)
        {
            Bitmap output = (Bitmap)picPreview.Image;

            saveFileDialog1.Filter = "RGB565 Binary (*.pic) | *.pic";
            DialogResult result = saveFileDialog1.ShowDialog();

            if (saveFileDialog1.FileName != string.Empty)
            {
                // Create binary file
                using (FileStream stream = new FileStream(saveFileDialog1.FileName, FileMode.Create))
                {
                    using (BinaryWriter writer = new BinaryWriter(stream))
                    {
                        // Write header (U16 width, U16 height, U16 depth)
                        writer.Write(Convert.ToInt16(txtOutX.Text));     // Width
                        writer.Write(Convert.ToInt16(txtOutY.Text));     // Height
                        writer.Write((short)1);                          // 1 = RGB565

                        // Write pixel data
                        BitmapData bmpData = output.LockBits(new Rectangle(0, 0, Convert.ToInt32(txtOutX.Text) - 1, Convert.ToInt32(txtOutY.Text) - 1), ImageLockMode.ReadOnly, PixelFormat.Format16bppRgb565);
                        byte[] buffer = new byte[(Convert.ToInt32(txtOutX.Text) * Convert.ToInt32(txtOutY.Text)) * 2];
                        Marshal.Copy(bmpData.Scan0, buffer, 0, buffer.Length);
                        writer.Write(buffer, 0, buffer.Length);
                        output.UnlockBits(bmpData);

                        // Close binary file
                        writer.Close();

                        // Display message
                        MessageBox.Show("Export complete");
                    }
                }
            }
        }

        /// <summary>
        /// Handles the Click event of the btnImgSrc control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void btnImgSrc_Click(object sender, EventArgs e)
        {
            openFileDialog1.Filter = "Image Files | *.jpg; *.png; *.bmp";
            openFileDialog1.ShowDialog();
        }

        /// <summary>
        /// Handles the FileOk event of the openFileDialog1 control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.ComponentModel.CancelEventArgs"/> instance containing the event data.</param>
        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {
            txtFilename.Text = openFileDialog1.FileName;
            txtOutX.Text = string.Empty;
            txtOutY.Text = string.Empty;

            // Try to load basic image information
            try
            {
                Bitmap bmp = new Bitmap(txtFilename.Text);
                if (bmp != null)
                {
                    txtSrcWidth.Text = bmp.Width.ToString();
                    txtSrcHeight.Text = bmp.Height.ToString();

                    // Enable dropdowns
                    cmbColorDepth.Enabled = true;
                    cmbImageSize.Enabled = true;

                    // Set default values
                    cmbColorDepth.SelectedIndex = 0;
                    cmbImageSize.SelectedIndex = 0;

                    // Enable convert button
                    btnConvert.Enabled = true;
                }
            }
            catch (Exception)
            {
                // ToDo: Handle and display any exceptions
                throw;
            }
        }

        /// <summary>
        /// Handles the Click event of the btnConvert control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void btnConvert_Click(object sender, EventArgs e)
        {
            Bitmap bmp = new Bitmap(txtFilename.Text);
            Bitmap preview;
            Graphics gfx;

            // Set preview window to full size (may have been resized by previous image)
            picPreview.Height = 320;
            picPreview.Width = 240;

            // Resize image
            if (bmp != null)
            {
                switch (cmbImageSize.SelectedIndex)
                {
                    case 0:
                        // 240x320 (Keep Ratio)
                        if ((bmp.Height / bmp.Width) == 1.5)
                        {
                            // Image can be resized directly
                            preview = new Bitmap(240, 320, System.Drawing.Imaging.PixelFormat.Format16bppRgb565);
                            gfx = Graphics.FromImage(preview);
                            gfx.DrawImage(bmp, 0, 0, 240, 320);
                        }
                        else
                        {
                            int newWidth, newHeight;
                            newWidth = 240;
                            // Prevent using images internal thumbnail
                            bmp.RotateFlip(System.Drawing.RotateFlipType.Rotate180FlipNone);
                            bmp.RotateFlip(System.Drawing.RotateFlipType.Rotate180FlipNone);
                            // Determine appropriate width/height
                            newHeight = bmp.Height * 240 / bmp.Width;
                            if (newHeight > 320)
                            {
                                // Resize with height instead
                                newWidth = (bmp.Width * 320) / bmp.Height;
                                newHeight = 320;
                            }
                            Image thumb = bmp.GetThumbnailImage(newWidth, newHeight, null, IntPtr.Zero);
                            preview = new Bitmap(newWidth, newHeight, System.Drawing.Imaging.PixelFormat.Format16bppRgb565);
                            gfx = Graphics.FromImage(preview);
                            gfx.DrawImage(thumb, 0, 0);
                            thumb.Dispose();
                            // Resize preview window
                            picPreview.Width = newWidth;
                            picPreview.Height = newHeight;
                            // Display output image size
                            txtOutY.Text = newHeight.ToString();
                            txtOutX.Text = newWidth.ToString();
                        }
                        break;
                    case 1:
                        // 240x320 (Force Ratio)
                        preview = new Bitmap(240, 320, System.Drawing.Imaging.PixelFormat.Format16bppRgb565);
                        gfx = Graphics.FromImage(preview);
                        gfx.DrawImage(bmp, 0, 0, 240, 320);
                        txtOutY.Text = "320";
                        txtOutX.Text = "240";
                        break;
                    case 2:
                        // Custom Size
                        preview = new Bitmap(Convert.ToInt32(txtCustomX.Text), Convert.ToInt32(txtCustomY.Text), System.Drawing.Imaging.PixelFormat.Format16bppRgb565);
                        gfx = Graphics.FromImage(preview);
                        gfx.DrawImage(bmp, 0, 0, Convert.ToInt32(txtCustomX.Text), Convert.ToInt32(txtCustomY.Text));
                        // Resize preview window
                        picPreview.Width = Convert.ToInt32(txtCustomX.Text);
                        picPreview.Height = Convert.ToInt32(txtCustomY.Text);
                        // Display output image size
                        txtOutY.Text = txtCustomY.Text;
                        txtOutX.Text = txtCustomX.Text;
                        break;
                    default:
                        preview = new Bitmap(240, 320, System.Drawing.Imaging.PixelFormat.Format16bppRgb565);
                        break;
                }
                // Display preview
                picPreview.Image = preview;

                // Enable export buttons
                btnSaveBinary.Enabled = true;
            }
        }

        /// <summary>
        /// Handles the SelectedIndexChanged event of the cmbImageSize control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void cmbImageSize_SelectedIndexChanged(object sender, EventArgs e)
        {
            // Enable or disable custom X/Y depending on the selected value
            if (cmbImageSize.SelectedIndex == 2)
            {
                // Enable custom X/Y
                txtCustomX.Enabled = true;
                txtCustomY.Enabled = true;
            }
            else
            {
                // Disable custom X/Y
                txtCustomX.Enabled = false;
                txtCustomY.Enabled = false;
            }
        }
    }
}
