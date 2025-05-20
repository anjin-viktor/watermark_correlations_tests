import csv

def detect_watermark(file_name, thresholds):

    filesCnt = 0
    detectedCntLC = 0
    detectedCntNC = 0
    detectedCntCC = 0
    detectedCntPC = 0

    with open(file_name) as csvfile:
        reader = csv.reader(csvfile, quoting=csv.QUOTE_MINIMAL)
        for row in reader:

            if(float(row[1]) >= thresholds[0]):
                detectedCntLC += 1
            if(float(row[2]) >= thresholds[1]):
                detectedCntNC += 1
            if(float(row[3]) >= thresholds[2]):
                detectedCntCC += 1
            if(float(row[4]) >= thresholds[3]):
                detectedCntPC += 1

            filesCnt += 1

    return  [filesCnt, detectedCntLC, detectedCntNC, detectedCntCC, detectedCntPC]
