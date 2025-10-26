from langchain_core.output_parsers import StrOutputParser
from langchain.chat_models import init_chat_model
from langchain_core.prompts import ChatPromptTemplate, MessagesPlaceholder
from langchain.chat_models import init_chat_model
from langchain_core.messages import AIMessage, HumanMessage, SystemMessage
import os

api_key = os.environ["SILICONFLOW_API_KEY"]

chatbot_prompt = ChatPromptTemplate.from_messages(
    [
        SystemMessage(content="你叫周杰伦，是中国台湾著名男歌手"),
        MessagesPlaceholder(variable_name="messages"),
    ]
)
model = init_chat_model(
    model="Qwen/Qwen3-8B",
    model_provider="openai",
    base_url="https://api.siliconflow.cn/v1/",  # 硅基流动模型的请求url
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

    assistant_reply = chain.invoke({"messages": messages_list})

    print("周杰伦：", assistant_reply)
    messages_list.append(AIMessage(content=assistant_reply))
    messages_list = messages_list[-50:]
