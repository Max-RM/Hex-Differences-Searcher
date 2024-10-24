# ArtemSU 2024

import tkinter
from tkinter import ttk, filedialog
from datetime import datetime
from os.path import getsize, isfile
from platform import system

global programName, info
programName = "HEX Differences Searcher 1.0.0"
info = "Developed by ArtemSU. 2024 ©️ TNT ENTERTAINMENT inc."

class getFiles:
	def __init__(self, root):
		self.root = root
		self.root.title(programName)

		self.fileL1 = tkinter.Label(root, text = "Path 1:")
		self.fileL1.grid(row = 0, column = 0, padx = 10, pady = 10)
		self.fileE1 = ttk.Entry(root, width = 70)
		self.fileE1.grid(row = 0, column = 1, padx = 10, pady = 10)
		self.fileB1 = tkinter.Button(root, text = "BROWSE", command = self.browse1)
		self.fileB1.grid(row = 0, column = 2, padx = 10, pady = 10)

		self.fileL2 = tkinter.Label(root, text = "Path 2:")
		self.fileL2.grid(row = 1, column = 0, padx = 10, pady = 10)
		self.fileE2 = ttk.Entry(root, width = 70)
		self.fileE2.grid(row = 1, column = 1, padx = 10, pady = 10)
		self.fileB2 = tkinter.Button(root, text = "BROWSE", command = self.browse2)
		self.fileB2.grid(row = 1, column = 2, padx = 10, pady = 10)

		self.startB = tkinter.Button(root, text = "START", bg = "#bbbbbb", command = self.start, width = 80, height = 5)
		self.startB.grid(row = 2, columnspan = 3, pady = 10)
		self.info = tkinter.Text(root, height = 10, width = 70)
		self.info.grid(row = 3, columnspan = 3, padx = 10, pady = 10)
		self.end = tkinter.Label(root, text = info)
		self.end.grid(row = 4, column = 1, padx = 10, pady = 10)

	def browse1(self):
		path = filedialog.askopenfilename()
		if path:
			self.fileE1.delete(0, tkinter.END)
			self.fileE1.insert(0, path)

	def browse2(self):
		path = filedialog.askopenfilename()
		if path:
			self.fileE2.delete(0, tkinter.END)
			self.fileE2.insert(0, path)

	def start(self):
		oneFile = self.fileE1.get()
		twoFile = self.fileE2.get()
		if not (oneFile or twoFile):
			message = "ERROR: Select 2 files."
			self.info.insert(tkinter.END, message + "\n")
			self.info.see(tkinter.END)
			return
		message = f"Files:\nPath 1: {oneFile}\nPath 2: {twoFile}"
		self.info.insert(tkinter.END, message + "\n")
		self.info.see(tkinter.END)
		message = "The program has started working."
		self.info.insert(tkinter.END, message + "\n")
		self.info.see(tkinter.END)
		o = go(oneFile, twoFile)
		messages = [
			"The work is completed.",
			"The program log is written to file.",
			f"Total differences found: {o}"
			]
		for message in messages:
			self.info.insert(tkinter.END, message + "\n")
			self.info.see(tkinter.END)
		if o == 0:
			message = "NO DIFFERENCES WERE FOUND. THE FILES ARE THE SAME."
			self.info.insert(tkinter.END, message + "\n")
			self.info.see(tkinter.END)
		return

def go(origPath, newPath):
	strings = []
	timeNow = datetime.now()
	strings.append(f"{programName}\n")
	strings.append(f"Time: {timeNow}\n")
	strings.append(f"File 1: {origPath}\n")
	strings.append(f"Size of File 1: {getsize(origPath)}\n")
	strings.append(f"File 2: {newPath}\n")
	strings.append(f"Size of File 2: {getsize(newPath)}\n")
	strings.append("\nSEARCH FOR DIFFERENCES...\n")
	with open(origPath, "rb") as origFile, open(newPath, "rb") as newFile:
		o = 0
		origContent, newContent = origFile.read(), newFile.read()
		for i, (origByte, newByte) in enumerate(zip(origContent, newContent)):
			if origByte != newByte:
				number = hex(i).upper().replace("0X", "0x")
				string = f"{number} >> {origByte:02X} >> {newByte:02X}"
				strings.append(f"{string}\n")
				o += 1
	if o == 0:
		strings.append("NO DIFFERENCES WERE FOUND. THE FILES ARE THE SAME.")
	else:
		strings.append(f"{o} DIFFERENCES FOUND.")
	timeNow = timeNow.strftime("%d-%m-%Y_%H-%M-%S")
	logFile = f"HDS-Logs_{str(timeNow)}.txt"
	with open(logFile, "w") as file:
		file.writelines(strings)
	if system() == "Windows":
		from os import startfile
		startfile(logFile)
	elif system() == "Linux":
		from subprocess import run
		run(["xdg-open", logFile])
	elif system() == "Darwin":
		from subprocess import run
		run(["open", logFile])
	return o

if __name__ == "__main__":
	root = tkinter.Tk()
	application = getFiles(root)
	root.mainloop()