import os
import shutil

from concurrent.futures import ThreadPoolExecutor
from watermarks import dct_watermark
from pathlib import Path
from tests import filter

def filter_watermark(watermark, watermark_name, embedding_level, input_filename, filter_name, filter_params):
    name_prefix = "correlation_tests/" + watermark_name + "/" + str(embedding_level)

    os.makedirs("correlation_tests", exist_ok=True)
    os.makedirs("correlation_tests/" + watermark_name, exist_ok=True)
    os.makedirs(name_prefix, exist_ok=True)
    os.makedirs(name_prefix + "/" + filter_name, exist_ok=True)

    filename = Path(input_filename)
    filename_wo_ext = filename.with_suffix('')

    watermarked_name = name_prefix + "/watermarked/" + str(filename_wo_ext) + ".bmp"
    filtered_name = name_prefix + "/" + filter_name + "/" + str(filename_wo_ext) + ".bmp"

    filter.run(filter_params, watermarked_name, filtered_name)

def filter_watermarks_multithread(watermark, watermark_name, embedding_level, filter_name, filter_params):
    threadPool = ThreadPoolExecutor()

    directory = os.fsencode("images/")

    futures = []
    for file in os.listdir(directory):
        filename = os.fsdecode(file)
        future = threadPool.submit(filter_watermark, watermark, watermark_name, embedding_level, filename, filter_name, filter_params)
        futures.append(future)

    for future in futures:
        future.result()

def filter_watermarks(watermark, watermark_name, embedding_level, filter_name, filter_params):
    filter_watermarks_multithread(watermark, watermark_name, embedding_level, filter_name, filter_params)

def remove_files(watermark, watermark_name, embedding_level, filter_name):
    dir_name = "correlation_tests/" + watermark_name + "/" + str(embedding_level) + "/" + filter_name
    shutil.rmtree(dir_name)


if __name__ == "__main__":
    embedding_level = 5
    filter_watermarks(dct_watermark, "dct_watermark", embedding_level, "blur", "unsharp=3:3:-0.25:3:3:-0.25")
#    remove_files(dct_watermark, "dct_watermark", embedding_level, "blur")
