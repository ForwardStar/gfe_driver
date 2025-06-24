import os
import time
import threading
import tarfile
import gzip
import sys

headers = {'User-Agent':'Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.6) Gecko/20091201 Firefox/3.5.6'}

def download(url, path):
    try:
        from pathlib import Path
        from tqdm import tqdm
    except:
        print("Installing dependencies...")
        from pip._internal import main
        main(['install', 'pathlib'])
        main(['install', 'tqdm'])
        from pathlib import Path
        from tqdm import tqdm
    from urllib.request import urlopen, Request
    print("Fetching from", url + "...")
    path = Path(path)
    blocksize = 1024 * 8
    blocknum = 0
    retry_times = 0
    while True:
        try:
            with urlopen(Request(url, headers=headers), timeout=3) as resp:
                total = resp.info().get("content-length", None)
                with tqdm(
                    unit="B",
                    unit_scale=True,
                    miniters=1,
                    unit_divisor=1024,
                    total=total if total is None else int(total),
                ) as t, path.open("wb") as f:
                    block = resp.read(blocksize)
                    while block:
                        f.write(block)
                        blocknum += 1
                        t.update(len(block))
                        block = resp.read(blocksize)
            break
        except KeyboardInterrupt:
            if path.is_file():
                path.unlink()
            raise
        except:
            retry_times += 1
            if retry_times >= 20:
                break
            print("Timed out, retrying...")
    if retry_times >= 20:
        if path.is_file():
            path.unlink()
        raise ConnectionError("bad internet connection, check it and retry.")

def showProcess():
    print(" ", end='', flush=True)
    while is_finished is False:
        print('\b-', end='', flush=True)
        time.sleep(0.05)
        print('\b\\', end='', flush=True)
        time.sleep(0.05)
        print('\b|', end='', flush=True)
        time.sleep(0.05)
        print('\b/', end='', flush=True)
        time.sleep(0.05)
    if is_finished is True:
        print('\bdone')
    else:
        print('\berror!')

if __name__ == "__main__":
    for argv in sys.argv:
        if argv == "--help" or argv == "-help" or argv == "-h":
            print("Usage:", "python3 downloader.py [options]")
            print("[options]:")
            print("   --help, -help, -h:              print help message")
            print("   --clean-raw:                    clean extracted datasets but keep original downloaded files")
            print("   --clean-all                     clean all extracted and downloaded files")
            exit()
        if argv.startswith('--'):
            if argv == "--clean-raw":
                if os.path.isdir("datasets"):
                    for file in os.listdir("datasets"):
                        if not file.endswith(".gz") and not file.endswith(".el"):
                            os.remove(os.path.join("datasets", file))
                print("Cleaned all extracted datasets.")
                exit()
            if argv == "--clean-all":
                if os.path.isdir("datasets"):
                    os.removedirs("datasets")
                print("Cleaned all extracted and downloaded files.")
                exit()

    # download datasets
    DATASETS_URL = ["https://zenodo.org/records/3966439/files/datasets.tar.gz",
                    "https://snap.stanford.edu/data/bigdata/communities/com-lj.ungraph.txt.gz",
                    "https://snap.stanford.edu/data/bigdata/communities/com-orkut.ungraph.txt.gz"]
    if os.path.isdir("datasets") is False or len(os.listdir("datasets")) < len(DATASETS_URL):
        print("Downloading datasets...")
        if os.path.isdir("datasets") is False:
            os.mkdir("datasets")
        for url in DATASETS_URL:
            path = os.path.join("datasets", url.split('/')[-1])
            if not os.path.exists(path):
                download(url, path)

    # extract all datasets
    print("Extracting datasets...", end=" ", flush=True)
    is_finished = False
    thread_extract_datasets = threading.Thread(target=showProcess)
    thread_extract_datasets.start()
    file_ls = os.listdir("datasets")
    for file in file_ls:
        if file.endswith(".tar.bz2") and not os.path.exists(os.path.join("datasets", '.'.join(file.split(".")[:-2]))):
            archive = tarfile.open(os.path.join("datasets", file), "r:bz2")
            archive.extractall("datasets")
        elif file.endswith(".tar.gz") and not os.path.exists(os.path.join("datasets", '.'.join(file.split(".")[:-2]))):
            archive = tarfile.open(os.path.join("datasets", file), "r:gz")
            archive.extractall("datasets")
        elif file.endswith(".txt.gz") and not os.path.exists(os.path.join("datasets", '.'.join(file.split(".")[:-1]))):
            archive = gzip.GzipFile(os.path.join("datasets", file))
            out = open(os.path.join("datasets", '.'.join(file.split(".")[:-1])), "wb")
            out.write(archive.read())
            archive.close()
    is_finished = True
    thread_extract_datasets.join()