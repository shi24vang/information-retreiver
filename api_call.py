import sys
from openai import OpenAI


def read_arguments():
    if len(sys.argv) < 4:
        print("Usage: python3 api_call.py API_KEY num_paragraphs query.txt")
        sys.exit(1)
    api_key = sys.argv[1]
    num_paragraphs = int(sys.argv[2])
    query_file = sys.argv[3]
    return api_key, num_paragraphs, query_file


def gather_paragraphs(count):
    blocks = []
    for idx in range(count):
        filename = f"paragraph_{idx}.txt"
        print(filename)
        with open(filename, "r") as handle:
            blocks.append(handle.read())
            blocks.append("\n")
    return blocks


def main():
    api_key, num_paragraphs, query_file = read_arguments()
    client = OpenAI(api_key=api_key)
    print(num_paragraphs)
    chunks = gather_paragraphs(num_paragraphs)
    with open(query_file, "r") as handle:
        chunks.append(handle.read())
        chunks.append("\n")
    payload = "\n".join(chunks)
    print(payload)
    chat = client.chat.completions.create(
        model="gpt-3.5-turbo",
        messages=[{"role": "user", "content": payload}],
    )
    reply = chat.choices[0].message.content
    try:
        with open("response.txt", "w") as handle:
            handle.write(reply)
        print("Data has been written to the file successfully!")
    except IOError:
        print("An error occurred while writing to the file.")
    print(reply)


if __name__ == "__main__":
    main()
