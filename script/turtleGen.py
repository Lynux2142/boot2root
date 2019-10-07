#!/usr/bin/python2.7

import sys

def print_begin(gen):
    gen.write('#!/usr/bin/python2.7\n\n')
    gen.write('from turtle import Turtle\n')
    gen.write('import time\n\n')
    gen.write('def main():\n')
    gen.write('    draw = Turtle()\n')
    gen.write('    draw.speed(0)\n')
    gen.write('    draw.left(90)\n')

def swap_language(mystr):
    if (mystr == '\n'):
        return ('    time.sleep(2)\n    draw.reset()\n\n    draw.left(90)\n')
    elif (not mystr.find('Can')):
        return ('\n')
    else:
        mystr = mystr.replace(' degrees', ')')
        mystr = mystr.replace(' spaces', ')')
        mystr = mystr.replace('Avance ', '    draw.forward(')
        mystr = mystr.replace('Recule ', '    draw.back(')
        mystr = mystr.replace('Tourne droite de ', '    draw.right(')
        mystr = mystr.replace('Tourne gauche de ', '    draw.left(')
    return (mystr)

def print_middle(gen, turtle):
    mystr = turtle.readline()
    strend = ''
    while (mystr):
        strend = strend + swap_language(mystr)
        mystr = turtle.readline()
    gen.write(strend)

def print_end(gen):
    gen.write("if __name__ == '__main__':\n")
    gen.write('    main()\n')

def main():
    try :
        turtle = open('turtle', 'r')
    except:
        print("error: can't read turtle file")
        sys.exit()
    gen = open('turtle_script.py', 'wa')
    print_begin(gen)
    print_middle(gen, turtle)
    print_end(gen)
    gen.close()
    turtle.close()

if __name__ == '__main__':
    main()
