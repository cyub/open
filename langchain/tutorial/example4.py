from langchain.chat_models import init_chat_model
from langchain_core.prompts import ChatPromptTemplate
from langchain_core.output_parsers import StrOutputParser
import os

api_key = os.environ["SILICONFLOW_API_KEY"]

model = init_chat_model(
    model="Qwen/Qwen3-8B",
    model_provider="openai",
    base_url="https://api.siliconflow.cn/v1/",  # 硅基流动模型的请求url
    api_key=api_key,
)

prompt_template = ChatPromptTemplate(
    [
        ("system", "你是一个乐意助人的助手，请根据用户的问题给出回答"),
        ("user", "这是用户的问题： {topic}，请用 yes 或 no 来回答"),
    ]
)


tool_qa_chain = prompt_template | model | StrOutputParser()
question = "请问 1 + 1 是否 大于 2？"

result = tool_qa_chain.invoke({"topic": question})

print(result)
