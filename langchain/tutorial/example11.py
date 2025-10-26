from langchain_core.output_parsers import StrOutputParser
from langchain.chat_models import init_chat_model
from langchain_core.prompts import ChatPromptTemplate, MessagesPlaceholder
from langchain.chat_models import init_chat_model
from langchain_core.messages import AIMessage, HumanMessage, SystemMessage
import os

api_key = os.environ["DEEPSEEK_API_KEY"]

chatbot_prompt = ChatPromptTemplate.from_messages(
    [
        SystemMessage(content="你叫周杰伦，是中国台湾著名男歌手"),
        MessagesPlaceholder(variable_name="messages"),
    ]
)


model = init_chat_model(
    model="deepseek-chat",  # deepseek-chat表示调用DeepSeek-v3模型，deepseek-reasoner表示调用DeepSeek-R1模型，
    model_provider="deepseek",  # 模型提供商写deepseek
    api_key=api_key,
)


parser = StrOutputParser()
chain = chatbot_prompt | model | parser

messages_list = []

print("> 输入 exit 结束对话")

while True:
    user_query = input("你：")
    if user_query.lower() in {"exit", "quit"}:
        break
    messages_list.append(HumanMessage(content=user_query))

    assistant_reply = ""
    print("周杰伦：", end=" ")
    for chunk in chain.stream({"messages": messages_list}):
        assistant_reply += chunk
        print(chunk, end="", flush=True)
    print()

    messages_list.append(AIMessage(content=assistant_reply))
    messages_list = messages_list[-50:]
