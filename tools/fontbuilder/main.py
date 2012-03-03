import Image
import ImageFont
import ImageDraw
from argparse import ArgumentParser
from os.path import basename


def buildFont(path, size, fontsize, offset, buffer, lines, widths):
  image = Image.new('RGBA', (size, size))
  draw = ImageDraw.Draw(image)

  base = basename(path)[0:-4]

  font = ImageFont.truetype(path, fontsize)
  f = open('{0}.dat'.format(base), 'w')

  for y in range(16):
    for x in range(16):
      c = x + y * 16
      s = size / 16
      w = font.getsize(chr(c))[0]
      f.write('{0}\n'.format(w))
      draw.text((x * s, y * s + offset), chr(c), font=font)
      if lines:
        draw.line([x * s, y * s + s, x * s + s, y * s + s, x * s + s, y * s], fill='black')
      if widths:
        draw.line([x * s + w, y * s, x * s + w, y * s + s], fill='black')

  image.save('{0}.png'.format(base), 'png')


if __name__ == '__main__':
  parser = ArgumentParser(description='Create A Bitmap Font', add_help=True)
  parser.add_argument('fontpath', help='Path to the font we\'re converting.')
  parser.add_argument('-o', '--offset', default=0, type=int, help='Vertical offsets of the pixels')
  parser.add_argument('-s', '--size', default=2048, type=int, help='Size of the image.  Defaults to 2048 pixels.')
  parser.add_argument('-f', '--fontsize', default=100, type=int, help='Font size to place on the image.')
  parser.add_argument('-l', '--lines', default=False, action='store_true', help='Draw lines for aligning.')
  parser.add_argument('-w', '--widths', default=False, action='store_true', help='Draw font width lines')
  parser.add_argument('-b', '--buffer', default=0, type=int, help='Add font-width buffer')

  args = parser.parse_args()
  buildFont(args.fontpath, args.size, args.fontsize, args.offset, args.buffer, args.lines, args.widths)
