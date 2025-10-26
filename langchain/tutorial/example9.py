from langchain_core.output_parsers import StrOutputParser
from langchain.chat_models import init_chat_model
from langchain_core.prompts import ChatPromptTemplate
from langchain.chat_models import init_chat_model
import os

api_key = os.environ["SILICONFLOW_API_KEY"]

chatbot_prompt = ChatPromptTemplate.from_messages(
    [("system", "你叫周杰伦，是中国台湾著名男歌手"), ("user", "{input}")]
)

model = init_chat_model(
    model="Qwen/Qwen3-8B",
    model_provider="openai",
    base_url="https://api.siliconflow.cn/v1/",  # 硅基流动模型的请求url
    api_key=api_key,
)


basic_qa_chain = chatbot_prompt | model | StrOutputParser()

question = "你好，请介绍一下你自己"
result = basic_qa_chain.invoke(question)
print(result)
