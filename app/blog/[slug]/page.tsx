import { getAllPosts, getPostBySlug } from "@/lib/posts";
import { MDXRemote } from 'next-mdx-remote/rsc'

export async function generateStaticParams() {
  const posts = getAllPosts(["slug"]);
  return posts.map((post) => ({ slug: post.slug }));
}

export default async function Page({ params }: { params: { slug: string } }) {
  const post = getPostBySlug(params.slug, ["title", "content"]);
  return (
    <MDXRemote
      source={post.content}
    />
  )

}
