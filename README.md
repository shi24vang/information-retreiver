# Information Retriever (C++)
Passage-search engine that returns exact book, page, and paragraph locations for arbitrary queries. It uses radix-trie and AVL indexes with rolling-hash substring search to keep lookup/update costs practical, ranks with RAKE keywords + a TextRank-style graph, and can optionally hand the top‑k hits to GPT‑3.5 for summaries.

## Project Layout
```
corpus/mahatma-gandhi-collected-works-volume-*.txt  # 98 books (required)
unigram_freq.csv                                    # background word statistics
tester.cpp                                          # entry point: ingestion + query
qna_tool.*                                          # paragraph retrieval + LLM bridge
dict.* / search.*                                   # supporting components
api_call.py / requirements.txt                      # OpenAI ChatCompletion runner
Makefile                                            # GCC/Clang build
```

## Prerequisites
- macOS/Linux with make and a GCC toolchain that provides `bits/stdc++.h` (e.g. `brew install gcc` on macOS).
- Python 3.9+ with `pip install -r requirements.txt` if you want the LLM step (installs `openai`).
- An OpenAI API key exported as `OPENAI_API_KEY` for the LLM path.

## Build
```bash
# from the repository root
make CC=g++-15        # use Homebrew g++ on macOS; drop CC=... on Linux
```
This compiles `Node.cpp`, `qna_tool.cpp`, `tester.cpp`, `dict.cpp`, and `search.cpp` into the `qna_tool` executable. Feel free to swap `g++-15` with whichever GCC version you installed.

## Basic Query Run (Paragraph Preview)
1. Edit `tester.cpp` and set the `question` string near the end to whatever you want to ask.
2. Rebuild: `make CC=g++-15`
3. Execute: `./qna_tool`

The binary reads every `corpus/mahatma-gandhi-collected-works-volume-*.txt`, indexes sentences, ranks the top five paragraphs for the question, and prints those paragraphs to stdout.

## Architecture Highlights
- **Radix trie + AVL postings**: `dict.*`/`qna_tool.*` lowercase every token, insert it into a radix trie, and hang AVL-balanced posting lists keyed by `(book_code, page, paragraph)`. This keeps insertions logarithmic and lets us retrieve exact `(book, page, paragraph)` tuples for any word in a query.
- **Rolling-hash substring search**: `search.*` maintains a Rabin–Karp index so you can verify literal string locations (offsets) if needed.
- **Keyword-driven ranking**: Queries flow through a RAKE-style keyword extractor, a heap-filtered paragraph fetch per keyword, and a TextRank-like graph that scores how well candidate paragraphs support each other. The simpler `get_top_k_para` path reuses the trie counts for lightweight ranking.
- **LLM summaries**: Once you have the top paragraphs, you can optionally call the GPT‑3.5 bridge to turn them into prose answers—matching the résumé bullet about GPT-3.5 summaries for top‑k hits.

## Sample Queries (Top-k IDs)
| Question | k | Top paragraphs (`book page paragraph`) |
| --- | --- | --- |
| What were the views of Mahatma Gandhi on the Partition of India? | 4 | `51 322 1`, `83 355 1`, `29 58 4`, `45 386 4` |
| Who was Mahatma Gandhi? | 7 | `51 322 1`, `83 355 1`, `29 58 4`, `79 457 2`, `45 386 4`, `29 226 4`, `9 484 4` |

Feel free to add more rows as you run additional checks—the tuples make it easy for reviewers to jump straight to the cited passages in the corpus.

## Example Run
```bash
make
./qna_tool
```
Example question in `tester.cpp`: `What is the date of birth of Mahatma Gandhi?`

Sample terminal output (truncated IDs):
```
Book_code: 51 Page: 322 Paragraph: 1
Book_code: 83 Page: 355 Paragraph: 1
Book_code: 29 Page: 58 Paragraph: 4
Book_code: 45 Page: 386 Paragraph: 4
Book_code: 79 Page: 457 Paragraph: 2
```
And corresponding paragraph previews:
```
Book_code: 51 Page: 322 Paragraph: 1
Book_code: 83 Page: 355 Paragraph: 1
Book_code: 29 Page: 58 Paragraph: 4
Book_code: 45 Page: 386 Paragraph: 4
Book_code: 79 Page: 457 Paragraph: 2
 not say that it is the Mahatma who is behind this resolution. The Working Committee is behind it; so you have to let go the Mahatma. It is far better that you do not vote for the resolution. It should not be that simply because some of us have done something we should be kept on. If we have proved ungrateful or if we have acted foolishly, then certainly remove us from office. And instead of removing us on our return from the Round Table Conference, you can do so now. We are your servants; we shall resign and get away. Even if there is the slightest self respect in us, we shall resign, and you will not be blamed for this. The world will say we did the right thing. But if you feel that because I am a Mahatma this cannot be done, then it is weakness on your part. If swaraj cannot be had without the Mahatma, then, believe me, you will never be able to rule yourselves. When I left for Dandi, I said that even if all the leaders were jailed the fight would not stop. Those outside would continue it. You have shown it in action. Not only all the leaders, but all the workers were jailed, yet our work did not stop. The women took it up and were in turn imprisoned; even then the work did not stop. Was Kikibehn Lalwani, who was dictator of Karachi, fit to be made dictator? The poor lady was suffering from phthisis. But she plunged into the battle, became dictator, and entered jail. The sick woman was cured, and she became a brave fighter. So, you can see that the Mahatma is not indispensable. I may add further that to the extent I deserve the title Mahatma, it was given to me for my spirit of service. Once I give up serving others I shall cease to be Mahatma from that very moment. Therefore do not feel that because this resolution was drawn up by the Mahatma, or by the Working Committee, therefore it cannot be opposed. If after deep consideration you reject this resolution then I shall defend you before the world. But if you accept that you must do exactly as you are told by those who have started the fight, then I tell you to be with me and put up a sincere fight. That is, do not worry if your spokesman returns empty handed. If he brings back something so much the better; but if he does not bring anything we shall fight again. So many have gone to jail, so many have faced lathi charges, and undergone hardships that we do not wish unnecessarily to repeat all this. But, having done our duty, if we do not get what we asked for, then our resolve to fight stands. And, when we are there those who remain


 resolution. He emphasized that what he wanted was not the recognition of Indian independence on paper, but in action. Asked if his movement would not hamper war efforts of the United Nations, Mahatma Gandhi said: The movement is intended not only to help China but also to make common cause with the Allies. On his attention being drawn to Mr. Amerys latest statement in the House of Commons, Mahatma Gandhi said: I am very much afraid that we shall have the misfortune to listen to a repetition of that language in stronger terms, but that cannot possibly delay the pace of the people or the group that is determined to go its way. Mahatma Gandhi added: There is no question of one more chance. After all, it is an open rebellion. Asked what form his movement would take, Mahatma Gandhi said: The conception is that of a mass movement on the widest possible scale. It will include what is possible to include in a mass movement or what people are capable of doing. This will be a mass movement of a purely non violent character. Asked if he would court imprisonment this time, Mahatma Gandhi said: It is too soft a thing. There is no such thing as courting imprisonment this time. My intention is to make it as short and swift as possible.A.P.I. 37. This message is an eye opener for me. I have often suffered from misreporting or coloured epitomes of my writings and speeches even to the point of being lynched 1. This one, though not quite as bad, is bad enough. The above A.P. summary gives, if it does, the clue to the authors source for the misquotation and the additional sentences. If he used that source, the question arises why he went out of his way to use that doubtful and unauthorized source, when he had before him the authentic text of the full interview in Harijan of 19th July last. He has made a most liberal, though disjointed and biased use of the columns of Harijan for building up his case against me. At page 13 of the indictment he thus begins the charge culminating in the misquotation at page 14. From this point onwards Mr. Gandhis conception of the struggle developed rapidly. His writings on the subject are too lengthy to quote in full, but the following excerpts from Harijan illustrate the direction in which his mind was moving. On the same page he has quoted passages from page 233 of Harijan from the report of the interview in question. I am, therefore,


 We have spoken so much today that it would not, I feel, be wrong if in deference to Sarojini Devis advice I hold my tongue. There is, however, one difficulty in doing so. I have left my weapon at home. If I had brought it, I would have given you an object lesson and asked you to take a wheel and start spinning along with me. I had no idea that I would receive this advice today from Sarojini Devi, or that it would be my lot to listen to so much praise. I am tired of listening to praise. Believe me when I say that I do not like this praise. But I shall not say more on this subject just now. I shall only say that I feel bound to those who have praised me and I request them all, as did Shri Jayakar, to help me by their silence. I shall be able to take up heavy responsibilities if I have your silent support. Before I say something, I wish to elicit atonement from one or two persons. Before we attend any meeting, we must learn good manners. We should know and respect the disposition of our guest. If you cannot do this, it is better that you do not attend the meeting. Two or three persons have violated this etiquette of a meeting. What Bhai Jamnadas 2 said was literally true. Much dirty work has been done in the shadow of Mahatma. The word Mahatma stinks in my nostrils; and, in addition to that, when somebody insists that everyone must call me Mahatma I get nausea, I do not wish to live. Had I not known that the more I insist on the word Mahatma, not being used, the more does it come into vogue, I would most certainly have insisted. In the Ashram where I live, every child, brother and sister has orders not to use the word Mahatma. None should refer to me as Mahatma


 The B.I.S.N. Co., in a statement to The Englishman, denies the allegations made by Mahatma Gandhi, who, referring to the sanitary arrangements of the boats playing to Rangoon, termed them as criminal disregard of the welfare and feelings of the human cargo. 1 The Company says that Mahatma Gandhi thought that a deck passenger enjoyed second class privileges. The space termed by Mahatma Gandhi as blackhole was the extra space place at the disposal of the deck passengers with the hatches open for allowing ventilation. As regards the sanitary arrangements, the Company says there are sufficient men to keep the latrines, etc., clean, and no complainthas been sent to the Commander about any inconvenience. There are arrangements for hospital but Mahatma Gandhi and his friends occupied it (?) on their return journey from Rangoon without permission. I wish I had the full text of the statement 2 before me. But if the Free Press telegram is a fair summary of the B.I.S.N. Companys agents statement, I am sorry for it. Instead of setting about correcting the disgraceful state of affairs, the agent has chosen to refute my very mild condemnation of the treatment of deck passengers based on personal observations. I hope I am not so stupid as to expect second class privileges for deck passengers but I do resent the unnatural gulf that separates the deck passenger from the saloon passenger. The deck passenger may not claim the luxurious conveniences provided for saloon passengers but he is entitled, whether he asks for it or not, to complete sanitary arrangements, and ample and clean accommod 


 2. They are habitual khadi wearers. 3. They believe in the necessity of communal unity and removal of untouchability in every shape and form. 4. They believe in the necessity of supporting village handicrafts and swadeshi in everything. 5. They believe that swaraj for the millions is unattainable without non violence. 6. They believe in the Bombay resolution of the All India Congress Committee. 7. They believe in an inevitable connection between the above mentioned points and non violence. Mahatma Gandhi stresses that nobody is obliged to court imprisonment merely as a matter of discipline. Civil disobedience thus becomes a matter of inviolable faith and not discipline. Congress does notat least Mahatma Gandhi does notexpect anybody to offer civil disobedience who does not believe in the urgency of it. Mahatma Gandhi considers it disgraceful for any Congressman to say that he offers civil resistance for the sake of mere discipline. He has further stressed that lukewarm adherence to the Congress brings us no nearer to our goal; nor can half hearted political belief in the Congress programme, he says, answer the purpose. Those who do constructive work are just as good as civil resisters and by their faith and devotion to service, he says, they are rendering greater service to the cause of civil resistance than civil resisters of doubtful complexion. Mahatma Gandhi has stressed that we shall reach our goal if civil resistance has the backing of the nation in the shape of conformity to the constructive programme. Quality is the thing which is required in the fight and not quantity; of course, both combined would be welcomed. The Hindu, 26 12 1940
```
If `OPENAI_API_KEY` is set, the run also writes `paragraph_<i>.txt`, `query.txt`, and `response.txt` (GPT‑3.5 answer). Inspect `response.txt` to read the summarized answer.

## LLM Answer Generation (Optional)
1. Ensure `OPENAI_API_KEY` is exported (`export OPENAI_API_KEY=sk-...`) and Python deps are installed.
2. Leave or un-comment `qna_tool.query(question, "api_call.py");` in `tester.cpp` so the pipeline writes `paragraph_<i>.txt` and `query.txt`.
3. Rebuild and run `./qna_tool`. After the top paragraphs are chosen you will see `python3 api_call.py "$OPENAI_API_KEY" <k> query.txt` executed automatically.

`api_call.py` concatenates the saved paragraphs and the question, posts them to `gpt-3.5-turbo`, and writes the reply to `response.txt`. Inspect that file to verify the LLM narrative; keep the `paragraph_<i>.txt` files if you need to show the supporting evidence.

### Quick verification loop
```bash
# single-shot run without exporting globally
OPENAI_API_KEY="sk-your-real-key" ./qna_tool
cat response.txt
```
If you have ChatGPT Plus but no API billing, you still need to enable pay‑as‑you‑go credits on https://platform.openai.com/account/billing before the API call will succeed.

### Common LLM issues
- `OPENAI_API_KEY environment variable is not set`: export the key in the same terminal as the binary.
- `insufficient_quota` / HTTP 429: add a payment method or top up API credits, then regenerate the key and rerun.
- Want to share the chain-of-thought? Commit only the paragraph/query files you feel comfortable with; the `.gitignore` already excludes them by default.

## Troubleshooting
- **Missing files**: Both ingestion and retrieval now read from `corpus/…`; ensure every book is present there.
- **`bits/stdc++.h` not found**: Use Homebrew GCC (`brew install gcc`) and run `make CC=g++-15`, or replace that include with explicit standard headers.
- **API key error**: If you see `OPENAI_API_KEY environment variable is not set`, export it in the same shell before running `./qna_tool`. If you see `insufficient_quota`, provision billing in the OpenAI dashboard.
