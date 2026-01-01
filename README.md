# Information Retriever (C++)

Paragraph retrieval engine for the Collected Works of Mahatma Gandhi. It builds a radix-trie with AVL posting lists, supports Rabin–Karp substring search, ranks paragraphs with RAKE keywords plus a lightweight TextRank-style graph, and can optionally pass the top results to a Python summarizer.

## Project Layout
- `corpus/mahatma-gandhi-collected-works-volume-*.txt` – required books (not tracked in git).
- `unigram_freq.csv` – background word frequency table.
- `tester.cpp` – entry point that ingests the corpus and runs a sample query.
- `qna_tool.*` – paragraph retrieval pipeline and optional summary bridge.
- `dict.*` / `search.*` – trie-based index and substring search helpers.
- `api_call.py` / `requirements.txt` – optional OpenAI ChatCompletion summarizer.
- `Makefile` – builds the `qna_tool` binary.

## Prerequisites
- macOS/Linux with `make` and a C++11 toolchain that provides `bits/stdc++.h` (Homebrew `gcc` on macOS works).
- The Gandhi corpus placed at `corpus/mahatma-gandhi-collected-works-volume-*.txt`.
- Optional summaries: Python 3.9+, `pip install -r requirements.txt`, and `OPENAI_API_KEY` exported.

## Build
```bash
make
```
This compiles `Node.cpp`, `qna_tool.cpp`, `tester.cpp`, `dict.cpp`, and `search.cpp` into the `qna_tool` executable.

## Run a Query
1. Update the `question` string near the end of `tester.cpp`.
2. Rebuild with `make`.
3. Execute `./qna_tool` to print the top-ranked paragraphs and their `(book, page, paragraph)` identifiers.
4. If `OPENAI_API_KEY` is set, the run also calls `api_call.py` to write `response.txt` (a short summary) alongside `paragraph_<i>.txt` snippets and `query.txt`.

## Troubleshooting
- **Missing corpus**: ensure every `mahatma-gandhi-collected-works-volume-*.txt` file is present in `corpus/`.
- **bits/stdc++.h not found**: install GCC (e.g., `brew install gcc`) or replace that include with explicit standard headers.
- **No summary output**: the summarizer is skipped unless `OPENAI_API_KEY` is set in the environment.
