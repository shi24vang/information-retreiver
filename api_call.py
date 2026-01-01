import sys
from openai import OpenAI


def read_arguments():
    if len(sys.argv) < 4:
        print("Usage: python3 api_call.py API_KEY num_paragraphs query.txt")
        sys.exit(1)
    api_key = sys.argv[1]
    try:
        num_paragraphs = int(sys.argv[2])
    except ValueError:
        print("num_paragraphs must be an integer")
        sys.exit(1)
    query_file = sys.argv[3]
    return api_key, num_paragraphs, query_file


def gather_paragraphs(count):
    blocks: list[str] = []
    for idx in range(count):
        filename = f"paragraph_{idx}.txt"
        try:
            with open(filename, "r") as handle:
                blocks.append(handle.read())
        except FileNotFoundError:
            continue
    return "\n\n".join(blocks)


def main():
    api_key, num_paragraphs, query_file = read_arguments()
    client = OpenAI(api_key=api_key)
    paragraphs = gather_paragraphs(num_paragraphs)
    with open(query_file, "r") as handle:
        query_text = handle.read()
    payload = paragraphs + "\n" + query_text
    chat = client.chat.completions.create(
        model="gpt-3.5-turbo",
        messages=[{"role": "user", "content": payload}],
    )
    reply = chat.choices[0].message.content
    try:
        with open("response.txt", "w") as handle:
            handle.write(reply)
        print("Summary written to response.txt")
    except IOError:
        print("An error occurred while writing to response.txt.")


if __name__ == "__main__":
    main()
