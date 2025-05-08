import os
import math

from concurrent.futures import ThreadPoolExecutor
from watermarks import dct_watermark
from pathlib import Path
import csv

def calc_correlation(watermark, watermark_name, embedding_level, input_filename, filter_name):
    name_prefix = "correlation_tests/" + watermark_name + "/" + str(embedding_level)

    filename = Path(input_filename)
    filename_wo_ext = filename.with_suffix('')

    reference_name = name_prefix + "/reference/" + str(filename_wo_ext) + ".bmp"
    watermarked_name = name_prefix + "/watermarked/" + str(filename_wo_ext) + ".bmp"
    if filter_name:
        watermarked_name = name_prefix + "/" + filter_name + "/" + str(filename_wo_ext) + ".bmp"

    resultLC = watermark.get_correlation(watermarked_name, reference_name, 0)
    resultCC = watermark.get_correlation(watermarked_name, reference_name, 2)
    resultPC = watermark.get_correlation(watermarked_name, reference_name, 3)

    return [filename, resultLC, resultCC, resultPC]

def calc_correlation_wo_embedding(watermark, watermark_name, embedding_level, input_filename):
    name_prefix = "correlation_tests/" + watermark_name + "/" + str(embedding_level)

    filename = Path(input_filename)
    filename_wo_ext = filename.with_suffix('')

    reference_name = name_prefix + "/reference_wo_embedding/" + str(filename_wo_ext) + ".bmp"
    watermarked_name = name_prefix + "/watermarked/" + str(filename_wo_ext) + ".bmp"

    resultLC = watermark.get_correlation(watermarked_name, reference_name, 0)
    resultCC = watermark.get_correlation(watermarked_name, reference_name, 2)
    resultPC = watermark.get_correlation(watermarked_name, reference_name, 3)

    return [filename, resultLC, resultCC, resultPC]

def get_results_file_name(watermark_name, embedding_level, filter_name):
    csv_file_name = "correlation_tests/" + watermark_name + "/" + str(embedding_level) + "/correlations_" + watermark_name + "_" + str(embedding_level) + ".csv"
    if filter_name:
        csv_file_name = "correlation_tests/" + watermark_name + "/" + str(embedding_level) + "/correlations_" + watermark_name + "_" + str(embedding_level) + "_" + filter_name + ".csv"

    return csv_file_name

def get_results_file_name_wo_embedding(watermark_name, embedding_level):
    csv_file_name = "correlation_tests/" + watermark_name + "/" + str(embedding_level) + "/correlations_wo_embedding_" + watermark_name + "_" + str(embedding_level) + ".csv"

    return csv_file_name


def calc_correlations_multithread(watermark, watermark_name, embedding_level, filter_name):
    threadPool = ThreadPoolExecutor()

    directory = os.fsencode("images/")

    futures = []
    for file in os.listdir(directory):
        filename = os.fsdecode(file)
        future = threadPool.submit(calc_correlation, watermark, watermark_name, embedding_level, filename, filter_name)
        futures.append(future)

    csv_file_name = get_results_file_name(watermark_name, embedding_level, filter_name)

    csv_file = open(csv_file_name, 'w', newline='', encoding='utf-8')
    writer = csv.writer(csv_file)

    for future in futures:
        result = future.result()
        writer.writerow(result)

def calc_correlations_wo_embedding_multithread(watermark, watermark_name, embedding_level):
    threadPool = ThreadPoolExecutor()

    directory = os.fsencode("images/")

    futures = []
    for file in os.listdir(directory):
        filename = os.fsdecode(file)
        future = threadPool.submit(calc_correlation_wo_embedding, watermark, watermark_name, embedding_level, filename)
        futures.append(future)

    csv_file_name = get_results_file_name_wo_embedding(watermark_name, embedding_level)

    csv_file = open(csv_file_name, 'w', newline='', encoding='utf-8')
    writer = csv.writer(csv_file)

    for future in futures:
        result = future.result()
        writer.writerow(result)


def calc_correlations(watermark, watermark_name, embedding_level, filter_name):
    calc_correlations_multithread(watermark, watermark_name, embedding_level, filter_name)

def calc_correlations_wo_embedding(watermark, watermark_name, embedding_level):
    calc_correlations_wo_embedding_multithread(watermark, watermark_name, embedding_level)

if __name__ == "__main__":
    embedding_level = 5
    calc_correlations(dct_watermark, "dct_watermark", embedding_level, "")
    calc_correlations(dct_watermark, "dct_watermark", embedding_level, "blur")
