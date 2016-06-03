import socket

def isHostnameAllowed():
	currentHost = socket.gethostname()
	testingHostsFile = 'testing_machines.txt'
	with open(testingHostsFile, 'r') as file:
		machines = [ x.strip().upper() for x in file.readlines() ]
		if currentHost.upper() in machines:
			return True
		else:
			return False
