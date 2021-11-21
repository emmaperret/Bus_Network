import serial

ser = serial.Serial('/dev/ttyAMA0', 115200) #ouvre le port série de la raspberry configuré avecle même baudrate que le port UART4 de la STM

#enregirstrer la commande donnée
inp = input()

#tester la commande donnée
if inp=='GET_T' :
	ser.write(b'GET_T')

if inp == 'GET_P' :
	ser.write(b'GET_P')
	
if inp == 'SET_K' :
	val = input() #enregistrer la valeur qu'on veut attribuer à K
	ser.write(b'SET_K' + val)
	
if inp == 'GET_K' :
	ser.write(b'GET_K')
	
	
 if inp == 'GET_A' :
	ser.write(b'GET_A')