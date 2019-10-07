#!/usr/local/bin/python3

import sys

def main():
    phase_1 = "Publicspeakingisveryeasy."
    phase_2 = "12624120720"
    phase_3 = [ "1b214", "2b755", "7b524" ]
    phase_4 = "9"
    phase_5 = []
    phase_6 = "426135" # mot de passe de base '4 2 6 3 1 5' on a swaper le 3 et le 1 (erreur dans le sujet)

    try:
        phase_5_file = open(sys.argv[1], 'r')
    except IOError as e:
        print('error: {}'.format(e))
        return
    except:
        print('error: {}'.format(sys.exc_info()))
        return
    line = phase_5_file.readline().strip('\n')
    while (line):
        phase_5.append(line)
        line = phase_5_file.readline().strip('\n')

    for i in phase_3:
        for j in phase_5:
            print(phase_1 + phase_2 + i + phase_4 + j + phase_6)

if __name__ == '__main__':
    main()
