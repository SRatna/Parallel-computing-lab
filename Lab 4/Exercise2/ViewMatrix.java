import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import javax.swing.*;
import java.io.*;

class ViewMatrix extends JComponent
{
	int[][] image = null;
	int n = 0;

	public ViewMatrix(String file)
	{
		BufferedReader reader = null;
		try {
			reader = new BufferedReader(new FileReader(file));
			
			String line = reader.readLine();
			int size = Integer.parseInt(line);
			int step;
			if (size < 800) {
				n = size;
				step = 1;
			}
			else {
				step = 1 + (size / 800);
				n = (size+step-1)/step;
			}

			image = new int[n][n];

			for (int i=0; i<size; i++) {
				for (int j=0; j<size; j++) {
					do {
						line = reader.readLine();
					} while (line.equals(""));
					if ((i % step == 0) && (j % step == 0)) {
						double d = Double.parseDouble(line);
						image[i/step][j/step] = gray(d);
					}
				}
			}
		}
		catch (FileNotFoundException e) {
			System.err.println("Could not open file " + file);
		}
		catch (Exception e) {
			System.err.println("I/O error");
		}
		finally {
			try {
				reader.close();
			}
			catch (Exception e) {}
		}

		setPreferredSize(new Dimension(n,n));
	}
		
	int gray(double v)
	{
		int r, g, b;
		if (v < 0)
			v = 0;
		if (v > 1)
			v = 1;
		r = (int)(v * 255 + .5) << 16;
		g = 0;
		b = (int)((1.0-v) * 255 + .5);

		return r+g+b;
	}

	public void paint(Graphics g)
	{
		if (image != null) {
			for (int i=0; i<n; i++) {
				int last_col = -1;
				int ja = 0;
				for (int j=0; j<n; j++) {
					if (image[i][j] != last_col) {
						if (j > 0)
							g.drawLine(i,ja,i,j-1);
						g.setColor(new Color(image[i][j]));
						last_col = image[i][j];
						ja = j;
					}
				}
				g.drawLine(i,ja,i,n-1);
			}
		}
	}

	public static void main(String args[])
	{
		if (args.length < 1) {
			System.err.println("Usage: java ViewMatric <filename>");
			return;
		}
		JFrame frame = new JFrame("View Matrix");
		ViewMatrix view = new ViewMatrix(args[0]);
		frame.getContentPane().add(view);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.pack();
		frame.setVisible(true);
	}
}
