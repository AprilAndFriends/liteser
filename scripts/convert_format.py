# Cateia Games Upsell Screen Graphics Assembler

import os
import shutil
import sys

from Model import *
from Util import *

VERSION = "1.0"

def process():
	print ""
	print "-------------------------------------------------------------------------------"
	print "| Liteser Format Converter " + VERSION
	print "-------------------------------------------------------------------------------"
	
	if len(sys.argv) != 5:
		help()
		return
	inputPath = sys.argv[1]
	outputPath = sys.argv[2]
	inputFormat = sys.argv[3]
	outputFormat = sys.argv[4]
	if not inputFormat in Model.Formats:
		print "ERROR: Format not support: " + inputFormat
		return
	if not outputFormat in Model.Formats:
		print "ERROR: Format not support: " + outputFormat
		return
	
	
	if os.path.isdir(inputPath):
		print ""
		print "-- Finding files..."
		print ""
		files = Util.getFileList(inputPath)
		#print files
		inputFiles = []
		outputFiles = []
		inputExtension = "." + inputFormat
		outputExtension = "." + outputFormat
		for file in files:
			if file.endswith(inputExtension):
				filename = file[len(inputPath) + 1: len(file) - len(inputExtension)]
				inputFiles.append(inputPath + "/" + filename + inputExtension)
				outputFiles.append(outputPath + "/" + filename + outputExtension)
	else:
		inputFiles = [inputPath]
		outputFiles = [outputPath]
		
	print ""
	print "-- Reading files..."
	print ""
	
	data = []
	for file in inputFiles:
		data.append(Model.readFile(inputFormat, file))
	##print inputFiles
	##print outputFiles
	
	
	
	print ""
	print "Done!"
	
def help():
	print ""
	print "This script converts files between the various Liteser formats."
	print ""
	print "usage:   convert_format.py INPUT_PATH OUTPUT_PATH INPUT_FORMAT OUTPUT_FORMAT"
	print ""
	print "  INPUT_PATH       - Input directory or filename. If directory, it will be"
	print "                     parsed recursively."
	print "  OUTPUT_PATH      - Output directory or filename. Should match INPUT_PATH's"
	print "                     path type to avoid problems."
	print "  INPUT_FORMAT     - The input format. See below for supported formats."
	print "  OUTPUT_FORMAT    - The output format. See below for supported formats."
	print ""
	print "Supported formats:"
	print "  ls2 - Liteser 2 Binary"
	print "  lsx - Liteser XML"
	print ""
	print "example: convert_format.py ../data data/ls ls2 lsx"
	print "example: convert_format.py characters.lsx \"data files/characters.ls2\" lsx ls2"
	print ""
	os.system("pause")

process()
