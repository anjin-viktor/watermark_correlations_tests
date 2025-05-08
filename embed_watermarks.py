import os
import shutil

from concurrent.futures import ThreadPoolExecutor
from watermarks import dct_watermark
from pathlib import Path

def embed_watermark(watermark, watermark_name, embedding_level, input_filename):
    name_prefix = "correlation_tests/" + watermark_name + "/" + str(embedding_level)

    os.makedirs("correlation_tests", exist_ok=True)
    os.makedirs("correlation_tests/" + watermark_name, exist_ok=True)
    os.makedirs(name_prefix, exist_ok=True)
    os.makedirs(name_prefix + "/watermarked/", exist_ok=True)
    os.makedirs(name_prefix + "/reference/", exist_ok=True)

    filename = Path(input_filename)
    filename_wo_ext = filename.with_suffix('')

    reference_name = name_prefix + "/reference/" + str(filename_wo_ext) + ".bmp"
    output_name = name_prefix + "/watermarked/" + str(filename_wo_ext) + ".bmp"
    input_file = "images/" + input_filename

    watermark.gen_reference(input_file, reference_name, embedding_level)
    watermark.embed(input_file, reference_name, output_name)

def embed_watermarks_multithread(watermark, watermark_name, embedding_level):
    threadPool = ThreadPoolExecutor()

    directory = os.fsencode("images/")

    futures = []
    for file in os.listdir(directory):
        filename = os.fsdecode(file)
        future = threadPool.submit(embed_watermark, watermark, watermark_name, embedding_level, filename)
        futures.append(future)

    for future in futures:
        future.result()

def create_reference_wo_embedding(watermark, watermark_name, embedding_level, input_filename):
    name_prefix = "correlation_tests/" + watermark_name + "/" + str(embedding_level)

    os.makedirs("correlation_tests", exist_ok=True)
    os.makedirs("correlation_tests/" + watermark_name, exist_ok=True)
    os.makedirs(name_prefix, exist_ok=True)
    os.makedirs(name_prefix + "/reference_wo_embedding/", exist_ok=True)

    filename = Path(input_filename)
    filename_wo_ext = filename.with_suffix('')

    reference_name = name_prefix + "/reference_wo_embedding/" + str(filename_wo_ext) + ".bmp"
    input_file = "images/" + input_filename

    watermark.gen_reference(input_file, reference_name, embedding_level)

def create_references_wo_embedding_multithread(watermark, watermark_name, embedding_level):
    threadPool = ThreadPoolExecutor()

    directory = os.fsencode("images/")

    futures = []
    for file in os.listdir(directory):
        filename = os.fsdecode(file)
        future = threadPool.submit(create_reference_wo_embedding, watermark, watermark_name, embedding_level, filename)
        futures.append(future)

    for future in futures:
        future.result()

def embed_watermarks(watermark, watermark_name, embedding_level):
    embed_watermarks_multithread(watermark, watermark_name, embedding_level)

def create_references_wo_embedding(watermark, watermark_name, embedding_level):
    create_references_wo_embedding_multithread(watermark, watermark_name, embedding_level)

def remove_files(watermark, watermark_name, embedding_level):
    dir_name = "correlation_tests/" + watermark_name + "/" + str(embedding_level) + "/watermarked/"
    shutil.rmtree(dir_name)

    dir_name = "correlation_tests/" + watermark_name + "/" + str(embedding_level) + "/reference/"
    shutil.rmtree(dir_name)

    dir_name = "correlation_tests/" + watermark_name + "/" + str(embedding_level) + "/reference_wo_embedding/"
    shutil.rmtree(dir_name)


if __name__ == "__main__":
    embedding_level = 5
    embed_watermarks(dct_watermark, "dct_watermark", embedding_level)
#    remove_files(dct_watermark, "dct_watermark", embedding_level)