from watermarks import dct_watermark
#from watermarks import eblind_dlc

import calc_correlations
import embed_watermarks
import filter_watermarks
import find_threshold
import detect_watermark

watermarks = { 
    "dct_watermark": dct_watermark 
#    ,"eblind_dlc": eblind_dlc 
}

levels = [5, 10, 15]

filters = {
    "blur": "unsharp=3:3:-0.25:3:3:-0.25",
    "dctdnoiz_5": "dctdnoiz=s=5",
    "dctdnoiz_10": "dctdnoiz=s=10",
    "sharp": "unsharp=3:3:0.25:3:3:0.25",
    "light": "colorlevels=rimax=0.902:gimax=0.902:bimax=0.902",
    "dark": "colorlevels=rimin=0.039:gimin=0.039:bimin=0.039:rimax=0.96:gimax=0.96:bimax=0.96",
    "contrast": "colorlevels=rimin=0.058:gimin=0.058:bimin=0.058",
    "bright": "colorlevels=romin=0.05:gomin=0.05:bomin=0.05",
    "noise": "noise=alls=25:allf=u",
    "nlmeans_10": "nlmeans=s=10",
    "nlmeans_5": "nlmeans=s=5",
    "pixelize": "pixelize=w=3:h=3",
    "fftdnoiz_15": "fftdnoiz=sigma=15",
    "fftdnoiz_5": "fftdnoiz=sigma=5"
}

for watermark_name, watermark in watermarks.items():
    for level in levels:
        print("%s: %d" % (watermark_name, level))
        embed_watermarks.embed_watermarks(watermark, watermark_name, level)
        embed_watermarks.create_references_wo_embedding(watermark, watermark_name, level)

        calc_correlations.calc_correlations(watermark, watermark_name, level, "")
        calc_correlations.calc_correlations_wo_embedding(watermark, watermark_name, level)

        thresholds = find_threshold.find_threshold(calc_correlations.get_results_file_name(watermark_name, level, ""), calc_correlations.get_results_file_name_wo_embedding(watermark_name, level))
        detected = detect_watermark.detect_watermark(calc_correlations.get_results_file_name(watermark_name, level, ""), thresholds)
        print("\tSelected thresholds: LC: %f CC: %f PC: %f " % (thresholds[0], thresholds[1], thresholds[2]))
        print("\tDetected unfiltered: %d %d %d of %d" % (detected[1], detected[2], detected[3], detected[0]))

        for filter_name, filter_params in filters.items():
            filter_watermarks.filter_watermarks(watermark, watermark_name, level, filter_name, filter_params,)
            calc_correlations.calc_correlations(watermark, watermark_name, level, filter_name)
            detected = detect_watermark.detect_watermark(calc_correlations.get_results_file_name(watermark_name, level, filter_name), thresholds)
            print("\tDetected after %s: %d %d %d of %d" % (filter_name, detected[1], detected[2], detected[3], detected[0]))

            filter_watermarks.remove_files(watermark, watermark_name, level, filter_name)

        embed_watermarks.remove_files(watermark, watermark_name, level)

