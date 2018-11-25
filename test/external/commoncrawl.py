# coding: utf-8

import getopt, sys, os, stat, subprocess
from multiprocessing import Process
import warc

warc_filename = "current.warc"
cmd_wget = ["wget", "-q",  "-O", warc_filename]
cmp_unzip = ["gzip", "-qq", "-k", "-d"]
commoncrawl_url = "https://commoncrawl.s3.amazonaws.com"

def run_test(file_list, bin_parser, save_to = ".", threads_count = 10):
    f = open(file_list, "r")

    for line in f:
        url = ("%s/%s" % (commoncrawl_url, line)).strip()

        print("Begin download: " + url)

        cmd_wget.append(url)
        proc = subprocess.run(cmd_wget)
        del cmd_wget[-1]

        if proc.returncode:
            continue

        print("Unzip file: " + warc_filename)

        cmp_unzip.append(warc_filename)
        proc = subprocess.run(cmd_wget)
        del cmp_unzip[-1]

        if proc.returncode:
            continue

        print("Begin process:" + url)

        parsed = process_warc(threads_count, bin_parser, warc_filename, url, save_to)

        print("Total parsed: " + parsed)

        os.remove(warc_filename)

def process_warc(threads_count, bin_parser, file_name, base_url, save_to = "."):
    warc_file = warc.open(file_name, 'rb')
    plist = []
    parsed = 0

    for idx, record in enumerate(warc_file):
        url = record.url
        if not url:
            continue

        payload = record.payload.read()
        header, html = payload.split(b'\r\n\r\n', maxsplit=1)

        if url and html:
            plist.append(Process(target=read_doc, args=[bin_parser, save_to, base_url, idx, html]))
            plist[-1].start()

            parsed += 1

        if (idx % 100) == 0:
            print("%s" % idx)

        if len(plist) == threads_count:
            for p in plist:
                p.join()

            plist = []

    for p in plist:
        p.join()

    warc_file.close()

    return parsed

def read_doc(bin_parser, save_to, base_url, idx, html):
    parser = subprocess.Popen([bin_parser], stdin=subprocess.PIPE, stdout=subprocess.PIPE)

    parser.stdin.write(html)
    parser.stdin.close()

    rdata = parser.stdout.read()
    parser.stdout.close()

    parser.wait()

    if parser.returncode != 0:
        fp_base = os.path.basename(base_url)
        filename = "%s/%s-%s.errlog" % (os.path.splitext(fp_base)[0], save_to, idx)

        f = open(os.path.normpath(filename), 'wb')
        f.write(html)
        f.close()

    return len(html), len(rdata), parser.returncode

run_test(file_list = "/Users/alexanderborisov/Downloads/warc.paths", 
         bin_parser = "../../build/test/lexbor/html/lexbor_html_commoncrawl", 
         save_to = ".", threads_count = 10)