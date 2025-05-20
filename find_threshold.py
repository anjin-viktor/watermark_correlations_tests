import csv
import calc_correlations

def find_threshold(file_name_with_embedding, file_name_without_embedding):
    minLC = -1
    minNC = -1
    minCC = -1
    minPC = -1

    maxLCWoWatermark = -1.0
    maxNCWoWatermark = -1.0
    maxCCWoWatermark = -1.0
    maxPCWoWatermark = -1.0

    avgLC = 0
    avgNC = 0
    avgCC = 0

    avgLCWoWatermark = 0
    avgNCWoWatermark = 0
    avgCCWoWatermark = 0

    filesCnt = 0

    with open(file_name_with_embedding) as csvfile:
        reader = csv.reader(csvfile, quoting=csv.QUOTE_MINIMAL)
        for row in reader:
            if(minLC == -1):
                minLC = float(row[1])
                minNC = float(row[2])
                minCC = float(row[3]) 
                minPC = float(row[4]) 

            if(float(row[1]) < minLC):
                minLC = float(row[1])
            if(float(row[2]) < minNC):
                minNC = float(row[2])
            if(float(row[3]) < minCC):
                minCC = float(row[3])
            if(float(row[4]) < minPC):
                minPC = float(row[4])

            avgLC += float(row[1])
            avgNC += float(row[2])
            avgCC += float(row[3])
            filesCnt += 1

    avgLC /= filesCnt
    avgNC /= filesCnt
    avgCC /= filesCnt

    with open(file_name_without_embedding) as csvfile:
        reader = csv.reader(csvfile, quoting=csv.QUOTE_MINIMAL)
        for row in reader:
            if(maxLCWoWatermark == -1):
                maxLCWoWatermark = float(row[1])
                maxNCWoWatermark = float(row[2])
                maxCCWoWatermark = float(row[3]) 

            if(float(row[1]) > maxLCWoWatermark):
                maxLCWoWatermark = float(row[1])
            if(float(row[2]) > maxNCWoWatermark):
                maxNCWoWatermark = float(row[2])
            if(float(row[3]) > maxCCWoWatermark):
                maxCCWoWatermark = float(row[3])
            if(float(row[4]) > maxPCWoWatermark):
                maxPCWoWatermark = float(row[4])

            avgLCWoWatermark += float(row[1])
            avgNCWoWatermark += float(row[2])
            avgCCWoWatermark += float(row[3])

    avgLCWoWatermark /= filesCnt
    avgNCWoWatermark /= filesCnt
    avgCCWoWatermark /= filesCnt


#    thresholdLC = (avgLC + avgLCWoWatermark) / 2
#    thresholdNC = (avgNC + avgNCWoWatermark) / 2
#    thresholdCC = (avgCC + avgCCWoWatermark) / 2

    thresholdLC = maxLCWoWatermark
    thresholdNC = maxNCWoWatermark
    thresholdCC = maxCCWoWatermark
    thresholdPC = maxPCWoWatermark
#    print([thresholdLC, thresholdNC, thresholdCC])
#    print("LinearCorrelation:")
#    print("\taverage correlation WO watermark: %f" % (avgLCWoWatermark))
#    print("\ttaverage correlation WITH watermark: %f" % (avgLC))
#    print("\tselected threshold: %f" % (thresholdLC))

#    print("NormalizedCorrelation:")
#    print("\taverage correlation WO watermark: %f" % (avgNCWoWatermark))
#    print("\taverage correlation WITH watermark: %f" % (avgNC))
#    print("\tselected threshold: %f" % (thresholdNC))

#    print("CorrelationCoefficient:")
#    print("\taverage correlation WO watermark: %f" % (avgCCWoWatermark))
#    print("\taverage correlation WITH watermark: %f" % (avgCC))
#    print("\tselected threshold: %f" % (thresholdCC))

    return [thresholdLC, thresholdNC, thresholdCC, thresholdPC]

if __name__ == "__main__":
    find_threshold(calc_correlations.get_results_file_name("dct_watermark", 5, ""), calc_correlations.get_results_file_name_wo_embedding("dct_watermark", 5))
