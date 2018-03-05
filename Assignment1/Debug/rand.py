START_X = 0.1
START_Y = 0.6
for i in range (7):
	print('[{} {}]'.format(START_X, float(i) / 10))
for j in range (3):
	for i in range (10):
		print('[{} {}]'.format(START_X + float(i)/10, START_Y + j * 1.0 + 1.0-float(i)/10))
	for i in range (10):
		print('[{} {}]'.format(START_X + 1.0 - float(i) / 10, START_Y + j*1.0 + 2.0 * float(i) / 10))