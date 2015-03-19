i = open('results.txt', 'r')


lines = []
for line in i.readlines():
	inLine = line.split(' ')

	outLine = ','.join(inLine)
	lines.append(outLine)

i.close()
o = open('processed_results.txt', 'w')
o.writelines(lines)
o.close()
